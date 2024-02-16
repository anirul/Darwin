#pragma once

#include "state_interface.h"
#include "frame/common/application.h"
#include "network_app.h"

namespace darwin::state {

    class StateLogin : public StateInterface {
    public:
        StateLogin(
            frame::common::Application& app, 
            std::unique_ptr<darwin::NetworkApp> network_app) :
            app_(app), network_app_(std::move(network_app)) {}
        ~StateLogin() override = default;
        void Enter() override;
        void Update(StateContext& state_context) override;
        void Exit() override;

    private:
        frame::common::Application& app_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
        std::unique_ptr<darwin::NetworkApp> network_app_;
    };

} // namespace darwin::state.
