#pragma once

#include "state_interface.h"
#include "darwin_client.h"
#include "input_acquisition.h"
#include "frame/common/application.h"
#include "frame/gui/draw_gui_interface.h"
#include "Common/world_simulator.h"
#include "Common/darwin_constant.h"
#include "modal_stats.h"
#include "Common/client_parameter.pb.h"
#include "overlay_play.h"

namespace darwin::state {

    class StatePlay : public StateInterface {
    public:
        StatePlay(
            frame::common::Application& app, 
            std::unique_ptr<darwin::DarwinClient> darwin_client);
        ~StatePlay() override = default;
        void Enter(const proto::ClientParameter& client_parameter) override;
        void Update(StateContext& state_context) override;
        void Exit() override;

    protected:
        frame::ProgramInterface& GetProgram();
        void UpdateUniformSpheres(
            frame::ProgramInterface& program,
            darwin::UniformEnum& uniforms);
        void UpdateUniformCamera(
            frame::ProgramInterface& program,
            const proto::Character& character);
        void UpdateMovement(proto::Character character);

    private:
        frame::common::Application& app_;
        std::unique_ptr<darwin::DarwinClient> darwin_client_;
        std::string user_name_;
        proto::ClientParameter client_parameter_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
        WorldSimulator& world_simulator_;
        // Camera parameters.
        float camera_yaw_ = 0.0f; // left-right
        float camera_pitch_ = -20.0f; // up-down
        float camera_distance_ = 5.0f;
        // Saved variables.
        glm::vec3 character_initial_forward_ = glm::vec3(0.0f);
        glm::vec3 character_forward_ = glm::vec3(0.0f);
        float last_mouse_wheel_ = 0.0f;
        InputAcquisition* input_acquisition_ptr_ = nullptr;
        // GUI.
        frame::gui::DrawGuiInterface* draw_gui_ = nullptr;
        darwin::modal::ModalStats* stats_window_ = nullptr;
        overlay::OverlayPlay* overlay_play_ptr_ = nullptr;
    };

} // namespace darwin::state.
