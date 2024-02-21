#include "state_play.h"

#include "state_disconnected.h"
#include "state_context.h"

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

    void StatePlay::Update(StateContext& state_context) {
        if (!darwin_client_->IsConnected()) {
            state_context.ChangeState(
                std::make_unique<StateDisconnected>(
                    app_, 
                    std::move(darwin_client_)));
        }
        else {
            // Store the server time to only update in case it has changed.
            static double server_time = 0.0;
            if (server_time != darwin_client_->GetServerTime()) {
                auto elements = darwin_client_->GetElements();
                auto characters = darwin_client_->GetCharacters();
                server_time = darwin_client_->GetServerTime();
            }
        }
    }

} // namespace darwin::state.
