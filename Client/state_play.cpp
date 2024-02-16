#include "state_play.h"

namespace darwin::state {

    StatePlay::StatePlay(
        frame::common::Application& app,
        std::unique_ptr<darwin::NetworkApp> network_app) :
        app_(app), network_app_(std::move(network_app)) {}

    void StatePlay::Enter() {
        logger_->info("Entered play state");
    }

    void StatePlay::Exit() {
        logger_->info("Exited play state");
    }

    void StatePlay::Update(StateContext& state_context) {
        network_app_->Run();
    }

} // namespace darwin::state.
