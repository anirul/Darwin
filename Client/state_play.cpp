#include "state_play.h"

#include <algorithm>

#include "state_context.h"
#include "state_disconnected.h"
#include "state_death.h"
#include "state_victory.h"
#include "overlay_play.h"
#include "Common/convert_math.h"
#include "Common/vector.h"
#include "overlay_state.h"

namespace darwin::state {

    StatePlay::StatePlay(
        frame::common::Application& app,
        audio::AudioSystem& audio_system,
        std::unique_ptr<darwin::DarwinClient> darwin_client)
        : app_(app),
          audio_system_(audio_system),
          darwin_client_(std::move(darwin_client)),
          world_simulator_(darwin_client_->GetWorldSimulator())
    {
    }

    void StatePlay::Enter(const proto::ClientParameter& client_parameter) {
        logger_->info("Entered play state");
        audio_system_.PlayMusic(proto::AUDIO_MUSIC_PLAY);
        client_parameter_ = client_parameter;
        auto unique_input = std::make_unique<InputAcquisition>();
        input_acquisition_ptr_ = unique_input.get();
        app_.GetWindow().SetInputInterface(std::move(unique_input));
        for (auto* plugin : app_.GetWindow().GetDevice().GetPluginPtrs()) {
            logger_->info(
                "\tPlugin: [{}] {}",
                (std::uint64_t)plugin,
                plugin->GetName().c_str());
            if (!draw_gui_) {
                draw_gui_ =
                    dynamic_cast<frame::gui::DrawGuiInterface*>(
                        plugin);
            }
        }
        if (!draw_gui_) {
            throw std::runtime_error("No draw gui interface plugin found?");
        }
#ifdef _DEBUG
        auto overlay_state = std::make_unique<overlay::OverlayState>(
            "overlay_state",
            client_parameter_,
            client_parameter_.overlay_state());
        overlay_state->SetStateName("state play");
        draw_gui_->AddOverlayWindow(
            glm::vec2(0.0f, 0.0f),
            app_.GetWindow().GetDevice().GetSize(),
            std::move(overlay_state));
#endif // _DEBUG
        auto overlay_play = std::make_unique<overlay::OverlayPlay>(
            "overlay_play",
            client_parameter_,
            client_parameter_.overlay_play());
        overlay_play_ptr_ = overlay_play.get();
        draw_gui_->AddOverlayWindow(
            glm::vec2(0.0f, 0.0f),
            app_.GetWindow().GetDevice().GetSize(),
            std::move(overlay_play));
        for (const auto& title : draw_gui_->GetWindowTitles()) {
            logger_->info("\tWindow: {}", title);
            if (!stats_window_) {
                frame::gui::GuiWindowInterface* window = 
                    &draw_gui_->GetWindow(title);
                stats_window_ = 
                    dynamic_cast<darwin::modal::ModalStats*>(window);
            }
        }
        if (stats_window_) {
            logger_->info(
                "\tStats window [{}]: {}", 
                stats_window_->GetName(),
                (std::uint64_t)stats_window_);
        }
    }

    void StatePlay::Exit() {
        logger_->info("Exited play state");
        app_.GetWindow().SetInputInterface(nullptr);
        input_acquisition_ptr_ = nullptr;
        draw_gui_->DeleteWindow("overlay_play");
#ifdef _DEBUG
        draw_gui_->DeleteWindow("overlay_state");
#endif // _DEBUG
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
        if (character_initial_forward_ == glm::vec3(0.0f) || 
            glm::any(glm::isnan(character_initial_forward_))) 
        {
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
        static double previous_time = GetTimeSecondNow();
        double now = GetTimeSecondNow();
        double delta_time = now - previous_time;
        previous_time = now;
        if (character.status_enum() == proto::STATUS_ON_GROUND) {
            bool modified = false;
            proto::Physic physic = character.physic();
            const auto planet_physic = world_simulator_.GetPlanet();
            proto::PlayerParameter player_parameter =
                world_simulator_.GetPlayerParameter();
            if (input_acquisition_ptr_->IsJumping()) {
                modified = true;
                audio_system_.PlaySound(proto::AUDIO_SOUND_JUMP);
                physic.mutable_position_dt()->CopyFrom(
                    physic.position_dt() +
                    (character.normal() * player_parameter.vertical_speed()));
                character.set_status_enum(proto::STATUS_JUMPING);
            }
            if (input_acquisition_ptr_->IsMoving()) {
                modified = true;
                // calculate the firction for the delta_time
                double friction_delta_time = 
                    player_parameter.friction() * 
                    delta_time / std::log(physic.mass());
                // calculate acceleration from friction and traget terminal 
                // velocity.
                double acceleration_delta_time = 
                    friction_delta_time * 
                    player_parameter.horizontal_speed() * 
                    player_parameter.horizontal_speed();
                // we need to current speed to get the quadratic friction
                double current_speed = Length(physic.position_dt());
                // we apply the friction even if we acceletare (accelaration 
                // doesnt make friction disapear) also we wont end in orbit 
                // this way.
                auto friction_vector = 
                    Normalize(physic.position_dt()) * 
                    friction_delta_time * current_speed * current_speed ;
                auto forward = Normalize(Glm2ProtoVector(character_forward_));
                auto right =
                    Normalize(Cross(character.normal(), forward));
                auto direction =
                    Normalize(
                        right * input_acquisition_ptr_->GetHorizontal() +
                        forward * input_acquisition_ptr_->GetVertical());
                // lets apply acceleration and friction
                physic.mutable_position_dt()->CopyFrom(
                    physic.position_dt() + 
                    direction * acceleration_delta_time - friction_vector);
            }
            // Apply the changes.
            if (modified) {
                character.mutable_physic()->CopyFrom(physic);
                world_simulator_.SetCharacter(character);
            }
            else {
                auto next_character = world_simulator_.GetCharacterByName(
                    character.name());
                auto next_physic = next_character.physic();
                double current_speed = Length(next_physic.position_dt());
                double friction_delta_time = 
                    player_parameter.friction() * delta_time;
                // Lets make firction stronger with speed (so we wont end up 
                // in orbit).
                double speed_multiply = 
                    1.0 - friction_delta_time * current_speed * current_speed;
                next_physic.mutable_position_dt()->CopyFrom(
                    next_physic.position_dt() * speed_multiply);
                next_character.mutable_physic()->CopyFrom(next_physic);
                world_simulator_.SetCharacter(next_character);
            }
        }
    }

    void StatePlay::Update(StateContext& state_context) {
        stats_window_->SetCharacters(world_simulator_.GetCharacters());
        auto sound_effect = 
            world_simulator_.GetSoundEffect(
                darwin_client_->GetCharacterName());
        if (sound_effect != SoundEffectEnum::SOUND_EFFECT_NONE) {
            if (sound_effect == SoundEffectEnum::SOUND_EFFECT_GOOD) {
                audio_system_.PlaySound(proto::AUDIO_SOUND_GOOD);
            }
            if (sound_effect == SoundEffectEnum::SOUND_EFFECT_BAD) {
                audio_system_.PlaySound(proto::AUDIO_SOUND_BAD);
            }
        }
        auto player_parameter = world_simulator_.GetPlayerParameter();
        // Check in case of disconnect.
        if (!darwin_client_->IsConnected()) {
            state_context.ChangeState(
                std::make_unique<StateDisconnected>(
                    app_,
                    audio_system_,
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
        double now = 
            std::chrono::duration<double>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        auto character_name = darwin_client_->GetCharacterName();
        auto character = 
            world_simulator_.GetCharacterByName(character_name);
        // Update the input acquisition.
        UpdateMovement(character);
        // Update the world simulator.
        world_simulator_.UpdateTime();
        // Get the list of potential hit.
        auto name = world_simulator_.GetPotentialHit(character);
        // In case this is valid (not empty and not earth).
        if ((name != "") && (name != "earth")) {
            // Send a report movement to the server.
            darwin_client_->ReportHit(name);
        }
        // Update the overlay.
        if (overlay_play_ptr_) {
            overlay_play_ptr_->SetCharacterName(character_name);
            overlay_play_ptr_->SetCharacters(
                world_simulator_.GetCharacters());
        }
        // Get the delta time.
        double delta_time = now - world_simulator_.GetLastServerUpdateTime();
        // Get the close uniforms from the world simulator.
        auto uniforms = world_simulator_.GetCloseUniforms(
            Normalize(character.physic().position()), delta_time);
        assert(uniforms.spheres.size() == uniforms.colors.size());
        auto& program = GetProgram();
        bool is_character_valid = false;
        // Update Uniforms.
        program.Use();
        UpdateUniformSpheres(program, uniforms);
        if (character.name() != character_name) 
        {
            logger_->info("Character {} not found", character_name);
        }
        else 
        {
            is_character_valid = true;
            UpdateUniformCamera(program, character);
        }
        program.UnUse();
        if (is_character_valid) {
            if (character.physic().mass() <= 1.0) {
                world_simulator_.RemoveCharacter(character_name);
                darwin_client_->RemovePreviousCharacter(character_name);
                state_context.ChangeState(
                    std::make_unique<StateDeath>(
                        app_,
                        audio_system_,
                        std::move(darwin_client_)));
            }
            if (character.physic().mass() >=
                player_parameter.victory_size()) 
            {
                world_simulator_.RemoveCharacter(character_name);
                darwin_client_->RemovePreviousCharacter(character_name);
                state_context.ChangeState(
                    std::make_unique<StateVictory>(
                        app_,
                        audio_system_,
                        std::move(darwin_client_)));
            }
        }
    }

} // namespace darwin::state.
