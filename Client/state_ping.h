#pragma once

#include <memory>

#include "state_interface.h"
#include "network_app.h"
#include "frame/common/application.h"
#include "frame/logger.h"
#include "frame/gui/draw_gui_interface.h"
#include "modal_ping.h"

namespace darwin::state {

    class StatePing : public StateInterface {
    public:
        StatePing(
            frame::common::Application& app, 
            std::unique_ptr<darwin::NetworkApp> network_app) :
            app_(app), network_app_(std::move(network_app)) {}
        ~StatePing() override = default;
        void Enter() override;
        void Update(StateContext& state_context) override;
        void Exit() override;

    private:
        frame::common::Application& app_;
        std::unique_ptr<darwin::NetworkApp> network_app_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
        frame::gui::DrawGuiInterface* draw_gui_ = nullptr;
        std::mutex mutex_;
        darwin::modal::ModalPingParams modal_ping_params_;
    };

} // namespace darwin::state.
