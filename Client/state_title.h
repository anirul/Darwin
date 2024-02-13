#pragma once

#include "frame/logger.h"
#include "state_interface.h"
#include "frame/common/application.h"

namespace darwin::state {

    class StateTitle : public StateInterface {
    public:
        StateTitle(frame::common::Application& app);
        ~StateTitle() override = default;
        void Enter() override;
        void Update() override;
        void Exit() override;

    private:
        frame::Logger& logger_ = frame::Logger::GetInstance();
        frame::common::Application& app_;
    };

} // namespace darwin::state.
