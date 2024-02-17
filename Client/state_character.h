#pragma once

#include <memory>

#include "state_interface.h"
#include "network_app.h"
#include "frame/common/application.h"

namespace darwin::state {

    class StateCharacter : public StateInterface {
    public:
        StateCharacter(
            frame::common::Application& app, 
            std::unique_ptr<darwin::NetworkApp> network_app) :
            app_(app), network_app_(std::move(network_app)) {}
        ~StateCharacter() override = default;

        void Enter() override;
        void Update(StateContext& state_context) override;
        void Exit() override;

        private:
            frame::common::Application& app_;
            std::unique_ptr<darwin::NetworkApp> network_app_;
    };

}  // namespace darwin::state.
