#include "state_play.h"

#include <algorithm>

#include "state_disconnected.h"
#include "state_context.h"
#include "Common/convert_math.h"
#include "Common/vector.h"

namespace darwin::state {

    StatePlay::StatePlay(
        frame::common::Application& app,
        std::unique_ptr<darwin::DarwinClient> darwin_client) :
        app_(app),
        darwin_client_(std::move(darwin_client)),
        world_simulator_(darwin_client_->GetWorldSimulator())
    {
    }

    void StatePlay::Enter() {
        logger_->info("Entered play state");
        auto unique_input = std::make_unique<InputAcquisition>();
        input_acquisition_ptr_ = unique_input.get();
        app_.GetWindow().SetInputInterface(std::move(unique_input));
    }

    void StatePlay::Exit() {
        logger_->info("Exited play state");
        app_.GetWindow().SetInputInterface(nullptr);
        input_acquisition_ptr_ = nullptr;
    }

    frame::ProgramInterface& StatePlay::GetProgram() {
        auto& level = app_.GetWindow().GetDevice().GetLevel();
        auto id = level.GetIdFromName("RayMarchingProgram");
        return level.GetProgramFromId(id);
    }

    void StatePlay::UpdateUniformSpheres(
        frame::ProgramInterface& program,
        darwin::UniformEnum& uniform_enum)
    {
        program.Uniform(
            "sphere_size",
            static_cast<int>(uniform_enum.spheres.size()));
        program.Uniform("sphere_pos", uniform_enum.spheres);
        program.Uniform("sphere_col", uniform_enum.colors);
    }

    void StatePlay::UpdateUniformCamera(
        frame::ProgramInterface& program,
        const proto::Character& character)
    {
        // Input values.
        glm::vec3 character_pos =
            glm::vec3(ProtoVector2Glm(character.physic().position()));
        glm::vec3 character_up =
            glm::vec3(ProtoVector2Glm(character.normal()));
        glm::vec2 mouse_position = input_acquisition_ptr_->GetMousePosition();
        float mouse_wheel = input_acquisition_ptr_->GetMouseWheel();

        // Get the mouse to values.
        camera_yaw_ = mouse_position.x * 0.4f;
        camera_pitch_ = -(mouse_position.y * 0.1f);
        camera_distance_ += (last_mouse_wheel_ - mouse_wheel) * 1.0f;
        last_mouse_wheel_ = mouse_wheel;

        // clamp values.
        camera_distance_ = std::clamp(camera_distance_, 5.0f, 20.0f);
        camera_pitch_ = std::clamp(camera_pitch_, -80.0f, -5.0f);

        // Create a forward vector for the character (from random).
        if (character_initial_forward_ == glm::vec3(0.0f)) {
            do {
                character_initial_forward_ = glm::normalize(RandomVec3());
            } while (
                glm::dot(character_initial_forward_, character_up) > 0.999f);
        }
        // Recompute the right and forward vectors.
        auto character_right =
            glm::normalize(
                glm::cross(character_initial_forward_, character_up));
        character_initial_forward_ =
            glm::normalize(glm::cross(character_up, character_right));

        // Compute the quaternion for the pitch and yaw.
        glm::quat yaw_quat =
            glm::angleAxis(
                glm::radians(camera_yaw_),
                character_up);
        glm::quat pitch_quat =
            glm::angleAxis(
                glm::radians(camera_pitch_),
                character_right);
        character_forward_ =
            glm::normalize(
                yaw_quat * character_initial_forward_);

        // Compute the camera position.
        auto camera_arm =
            glm::normalize(
                yaw_quat * pitch_quat * character_initial_forward_);
        auto camera_pos = character_pos - camera_arm * camera_distance_;

        // Set the uniform values.
        program.Uniform("camera_pos", camera_pos);
        program.Uniform("camera_up", character_up);
        program.Uniform("camera_target", character_pos);
    }

    void StatePlay::UpdateMovement(proto::Character character) {
        if (character.status_enum() == proto::STATUS_ON_GROUND) {
            bool modified = false;
            proto::Physic physic = character.physic();
            // Reset position delta time on the ground.
            physic.release_position_dt()->CopyFrom(
                CreateBasicVector3(0.0, 0.0, 0.0));
            proto::PlayerParameter player_parameter = 
                world_simulator_.GetPlayerParameter();
            if (input_acquisition_ptr_->IsJumping()) {
                modified = true;
                physic.mutable_position_dt()->CopyFrom(
                    Add(
                        physic.position_dt(),
                        MultiplyVector3ByScalar(
                            character.normal(), 
                            player_parameter.vertical_speed())));
                character.set_status_enum(proto::STATUS_JUMPING);
            }
            if (input_acquisition_ptr_->IsMoving()) {
                modified = true;
                auto forward = Normalize(Glm2ProtoVector(character_forward_));
                auto right = 
                    Normalize(CrossProduct(character.normal(), forward));
                auto direction = 
                    Normalize(
                        Add(
                            MultiplyVector3ByScalar(
                                right, 
                                input_acquisition_ptr_->GetHorizontal()), 
                            MultiplyVector3ByScalar(
                                forward, 
                                input_acquisition_ptr_->GetVertical())));
                physic.mutable_position_dt()->CopyFrom(
                    Add(
                        physic.position_dt(),
                        MultiplyVector3ByScalar(
                            direction,
                            player_parameter.vertical_speed())));   
            }
            // Apply the changes.
            if (modified) {
                *character.mutable_physic() = physic;
                world_simulator_.SetCharacter(character);
                darwin_client_->ReportMovement(character.name(), physic);
            }
        }
    }

    void StatePlay::Update(StateContext& state_context) {
        // Check in case of disconnect.
        if (!darwin_client_->IsConnected()) {
            state_context.ChangeState(
                std::make_unique<StateDisconnected>(
                    app_,
                    std::move(darwin_client_)));
            return;
        }
        // Store the server time to only update in case it has changed.
        static double server_time = 0.0;
        if (server_time != darwin_client_->GetServerTime()) {
            world_simulator_.UpdateData(
                darwin_client_->GetElements(),
                darwin_client_->GetCharacters(),
                darwin_client_->GetServerTime());
            server_time = darwin_client_->GetServerTime();
        }
        auto character_name = darwin_client_->GetCharacterName();
        auto character = world_simulator_.GetCharacterByName(character_name);
        // Update the input acquisition.
        UpdateMovement(character);
        // Update the world simulator.
        world_simulator_.UpdateTime();
        // Get the close uniforms from the world simulator.
        auto uniforms = world_simulator_.GetCloseUniforms(
            Normalize(character.physic().position()));
        assert(uniforms.spheres.size() == uniforms.colors.size());
        auto& program = GetProgram();
        // Update Uniforms.
        program.Use();
        UpdateUniformSpheres(program, uniforms);
        if (character.name() != character_name) {
            logger_->warn("Character {} not found", character_name);
        }
        else {
            UpdateUniformCamera(program, character);
        }
        program.UnUse();
    }

} // namespace darwin::state.
