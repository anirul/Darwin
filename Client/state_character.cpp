#include "state_character.h"

#include "state_context.h"
#include "state_play.h"
#include "state_server.h"

namespace darwin::state {

    void StateCharacter::Enter() {
        logger_->info("Entering character state");
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
            std::make_unique<modal::ModalCharacter>(
                "Select Character",
                modal_character_params_));
    }

    void StateCharacter::Update(StateContext& state_context) {
        if (modal_character_params_.button_result != 
            modal::ModalCharacterButton::None) {
            switch (modal_character_params_.button_result) {
                case modal::ModalCharacterButton::Create: {
                    if (darwin_client_->CreateCharacter(
                        modal_character_params_.name,
                        modal_character_params_.color)) {
                        state_context.ChangeState(
                            std::make_unique<StatePlay>(
                                app_, 
                                std::move(darwin_client_)));
                    }
                    else {
                        state_context.ChangeState(
                            std::make_unique<StateCharacter>(
                                app_, 
                                std::move(darwin_client_)));
                        break;
                    }
                    state_context.ChangeState(
                        std::make_unique<StatePlay>(
                            app_,
                            std::move(darwin_client_)));
                }
                    break;
                case modal::ModalCharacterButton::Cancel:
                    state_context.ChangeState(
                        std::make_unique<StateServer>(app_));
                    break;
            }
        }
    }

    void StateCharacter::Exit() {
        logger_->info("Exit character state");
    }

} // namespace darwin::state.
