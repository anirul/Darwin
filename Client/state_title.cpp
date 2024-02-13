#pragma once

#include "state_title.h"

#include "frame/file/file_system.h"

namespace darwin::state {

    StateTitle::StateTitle(frame::common::Application& app) : app_(app) {}

    void StateTitle::Enter() {
        logger_->info("Entering title state");
    }

    void StateTitle::Exit() {
        logger_->info("Exiting title state");
    }

    void StateTitle::Update() {
        // After the player hit any keys, we can switch to the next state.
    }

} // namespace darwin::state.
