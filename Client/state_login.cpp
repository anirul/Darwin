#include "state_login.h"

namespace darwin::state {

    StateLogin::StateLogin(frame::common::Application& app) : app_(app) {
        network_app_ = std::make_unique<darwin::NetworkApp>();
    }

    void StateLogin::Enter() {
        logger_->info("Entering login state");
    }

    void StateLogin::Update() {
        network_app_->Run();
    }

    void StateLogin::Exit() {
        logger_->info("Exit login state");
    }

} // namespace darwin::state.
