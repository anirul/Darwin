#include "state_disconnected.h"

#include "frame/common/application.h"

namespace darwin::state {

    void StateDisconnected::Enter() {
        logger_->info("State disconnected entered");
    }

    void StateDisconnected::Exit() {
        logger_->info("State disconnected exited");
    }

    void StateDisconnected::Update(StateContext& state_context) {}

} // namespace darwin::state.
