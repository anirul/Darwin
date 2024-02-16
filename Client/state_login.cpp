#include "state_login.h"

#include "state_context.h"

namespace darwin::state {

    void StateLogin::Enter() {
        logger_->info("Entering login state");
    }

    void StateLogin::Update(StateContext& state_context) {
        network_app_->Run();

    }

    void StateLogin::Exit() {
        logger_->info("Exit login state");
    }

} // namespace darwin::state.
