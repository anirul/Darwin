#include "state_play.h"

namespace darwin::state {

    StatePlay::StatePlay(
        frame::common::Application& app,
        std::unique_ptr<darwin::DarwinClient> darwin_client) :
        app_(app), darwin_client_(std::move(darwin_client)) {}

    void StatePlay::Enter() {
        logger_->info("Entered play state");
    }

    void StatePlay::Exit() {
        logger_->info("Exited play state");
    }

    void StatePlay::Update(StateContext& state_context) {}

} // namespace darwin::state.
