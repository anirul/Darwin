#pragma once

#include "state_interface.h"
#include "frame/common/application.h"
#include "frame/gui/draw_gui_interface.h"
#include "frame/logger.h"
#include "modal_resolution.h"
#include "darwin_client.h"
#include "Common/client_parameter.pb.h"

namespace darwin::state {

    class StateResolution : public StateInterface {
    public:
        StateResolution(
            frame::common::Application& app,
            audio::AudioSystem& audio_system);
        ~StateResolution() override = default;
        
    public:
        void Enter(const proto::ClientParameter& client_parameter) override;
        void Update(StateContext& state_context) override;
        void Exit() override;

    private:
        frame::Logger& logger_ = frame::Logger::GetInstance();
        frame::common::Application& app_;
        audio::AudioSystem& audio_system_;
        proto::ClientParameter client_parameter_;
        modal::ModalResolutionParams modal_resolution_params_;
        frame::gui::DrawGuiInterface* draw_gui_interface_ = nullptr;
        bool passed_ = false;
    };
    
} // namespace darwin::state.
