#include "state_ping.h"

#include <memory>

#include "modal_ping.h"
#include "state_context.h"
#include "state_character.h"
#include "state_disconnected.h"

namespace darwin::state {

    void StatePing::Enter() {
        logger_->info("Entering ping state");
        for (auto* plugin : app_.GetWindow().GetDevice().GetPluginPtrs()) {
            logger_->info(
                "\tPlugin: [{}] {}", 
                (std::uint64_t)plugin, 
                plugin->GetName().c_str());
            if (!draw_gui_) {
                draw_gui_ =
                    dynamic_cast<frame::gui::DrawGuiInterface*>(
                        plugin);
            }
        }
        if (!draw_gui_) {
            throw std::runtime_error("No draw gui interface plugin found?");
        }
        draw_gui_->AddModalWindow(
            std::make_unique<modal::ModalPing>(
                "Select Server",
                modal_ping_params_,
                [this] { 
                    modal::ModalPingParams params;
                    bool result = darwin_client_->Ping(45323);
                    if (result) {
                        params.ping_value = 45323;
                        params.button_result = modal::ModalPingButton::Pong;
                    }
                    else {
                        params.ping_value = 0;
                        params.button_result = modal::ModalPingButton::Cancel;
                    }
                    return params;
                }));
    }

    void StatePing::Update(StateContext& state_context) {
        if (modal_ping_params_.button_result != modal::ModalPingButton::None) {
            switch (modal_ping_params_.button_result) {
                case modal::ModalPingButton::Pong:
                    state_context.ChangeState(
                        std::make_unique<StateCharacter>(
                            app_, 
                            std::move(darwin_client_)));
                    break;
                case modal::ModalPingButton::Cancel:
                    state_context.ChangeState(
                        std::make_unique<StateDisconnected>(
                            app_, 
                            std::move(darwin_client_)));
                    break;
            }
        }
    }

    void StatePing::Exit() {
        logger_->info("Exiting ping state");
    }

} // namespace darwin::state.
