#include "state_disconnected.h"

#include "frame/common/application.h"

namespace darwin::state {

    StateDisconnected::StateDisconnected(
        frame::common::Application &app) : app_(app) {}

    void StateDisconnected::Enter() {
        logger_->info("State disconnected entered");
    }

    void StateDisconnected::Exit() {
        logger_->info("State disconnected exited");
    }

    void StateDisconnected::Update() {}

} // namespace darwin::state.
