#pragma once

#include "state_interface.h"
#include "network_app.h"
#include "frame/common/application.h"

namespace darwin::state {

    class StatePlay : public StateInterface {
    public:
        StatePlay(
            frame::common::Application& app, 
            std::unique_ptr<darwin::NetworkApp> network_app);
        ~StatePlay() override = default;
        void Enter() override;
        void Update() override;
        void Exit() override;

    private:
        frame::common::Application& app_;
        std::unique_ptr<darwin::NetworkApp> network_app_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
    };

} // namespace darwin::state.
