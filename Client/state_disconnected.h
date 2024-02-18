#pragma once

#include "state_interface.h"
#include "frame/common/application.h"
#include "frame/logger.h"
#include "modal_disconnected.h"
#include "network_app.h"
#include "frame/gui/draw_gui_interface.h"

namespace darwin::state {

    class StateDisconnected : public StateInterface {
    public:
        StateDisconnected(
            frame::common::Application& app, 
            std::unique_ptr<NetworkApp> network_app) :
            app_(app), network_app_(std::move(network_app)) {}
        ~StateDisconnected() override = default;
        void Enter() override;
        void Update(StateContext& state_context) override;
        void Exit() override;

    private:
        frame::common::Application& app_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
        frame::gui::DrawGuiInterface* draw_gui_ = nullptr;
        modal::ModalDisconnectedParams modal_disconnected_params_;
        std::unique_ptr<NetworkApp> network_app_;
    };

} // namespace darwin::state.
