#include "state_title.h"

#include <memory>

#include "frame/file/file_system.h"
#include "state_server.h"
#include "state_context.h"

namespace darwin::state {

    StateTitle::StateTitle(frame::common::Application& app) : app_(app) {}

    void StateTitle::Enter() {
        logger_->info("Entering title state");
        // start_time_ = std::chrono::system_clock::now();
        app_.GetWindow().AddKeyCallback(' ', [this] {
                logger_->info("Space key pressed");
                passed_ = true;
                return true;
            });
    }

    void StateTitle::Exit() {
        logger_->info("Exiting title state");
        app_.GetWindow().RemoveKeyCallback(' ');
    }

    void StateTitle::Update(StateContext& state_context) {
        // auto duration = std::chrono::system_clock::now() - start_time_;
        // if (duration > std::chrono::seconds(10)) {
            // logger_->info("10 seconds passed");
            // passed_ = true;
        // }
        if (passed_) {
            state_context.ChangeState(
                std::make_unique<StateServer>(app_));
        }
    }

} // namespace darwin::state.
