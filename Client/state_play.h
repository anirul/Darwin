#pragma once

#include "state_interface.h"
#include "darwin_client.h"
#include "frame/common/application.h"
#include "Common/world_simulator.h"

namespace darwin::state {

    class StatePlay : public StateInterface {
    public:
        StatePlay(
            frame::common::Application& app, 
            std::unique_ptr<darwin::DarwinClient> darwin_client);
        ~StatePlay() override = default;
        void Enter() override;
        void Update(StateContext& state_context) override;
        void Exit() override;

    private:
        frame::common::Application& app_;
        std::unique_ptr<darwin::DarwinClient> darwin_client_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
        WorldSimulator world_simulator_;
    };

} // namespace darwin::state.
