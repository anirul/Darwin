#pragma once

#include "state_interface.h"
#include "frame/common/application.h"
#include "network_app.h"

namespace darwin::state {

    class StateLogin : public StateInterface {
    public:
        StateLogin(frame::common::Application& app);
        ~StateLogin() override = default;
        void Enter() override;
        void Update() override;
        void Exit() override;

    private:
        frame::common::Application& app_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
        std::unique_ptr<darwin::NetworkApp> network_app_;
    };

} // namespace darwin::state.
