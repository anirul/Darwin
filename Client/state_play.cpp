#include "state_play.h"

#include "state_disconnected.h"
#include "state_context.h"
#include "Common/convert_math.h"

namespace darwin::state {

    StatePlay::StatePlay(
        frame::common::Application& app,
        std::unique_ptr<darwin::DarwinClient> darwin_client) :
        app_(app),
        darwin_client_(std::move(darwin_client)) {}

    void StatePlay::Enter() {
        logger_->info("Entered play state");
    }

    void StatePlay::Exit() {
        logger_->info("Exited play state");
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
        glm::vec3 character_pos = 
            glm::vec3(ProtoVector2Glm(character.physic().position()));
        glm::vec3 character_up = glm::normalize(character_pos - center_);
        auto temporary_up = glm::vec3(ProtoVector2Glm(character.g_normal()));
        if (forward_ == glm::vec3(0.0f)) {
            do {
                forward_ = RandomVec3();
            } while (glm::abs(glm::dot(character_up, forward_)) > 0.999f);
        }
        glm::vec3 right = glm::normalize(glm::cross(character_up, forward_));
        // Recalculate to ensure orthogonality.
        forward_ = glm::normalize(glm::cross(right, character_up)); 

        // Apply yaw rotation around the character's up vector
        glm::mat4 yaw_rot = 
            glm::rotate(
                glm::mat4(1.0f), 
                glm::radians(camera_yaw_), 
                character_up);
        forward_ = glm::vec3(yaw_rot * glm::vec4(forward_, 0.0f));

        // Caluculate the camera position.
        glm::vec3 camera_pos = character_pos - camera_distance_ * forward_;

        // Set the uniform values.
        program.Uniform("camera_pos", camera_pos);
        program.Uniform("camera_up", character_up);
        program.Uniform("camera_target", character_pos);
    }

    void StatePlay::Update(StateContext& state_context) {
        if (!darwin_client_->IsConnected()) {
            state_context.ChangeState(
                std::make_unique<StateDisconnected>(
                    app_,
                    std::move(darwin_client_)));
        }
        else
        {
            // Store the server time to only update in case it has changed.
            static double server_time = 0.0;
            if (server_time != darwin_client_->GetServerTime()) {
                world_simulator_.UpdateData(
                    darwin_client_->GetElements(),
                    darwin_client_->GetCharacters(),
                    darwin_client_->GetServerTime());
            }
            else {
                world_simulator_.UpdateTime();
            }
            auto uniforms = world_simulator_.GetUniforms();
            assert(uniforms.spheres.size() == uniforms.colors.size());
            auto& program = GetProgram();
            auto character_name = darwin_client_->GetCharacterName();
            auto character = world_simulator_.GetCharacterByName(character_name);
            if (uniforms.spheres.size()) {
                center_ = glm::vec3(uniforms.spheres[0]);
            }
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
    }

} // namespace darwin::state.
