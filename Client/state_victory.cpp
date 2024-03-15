#include "state_victory.h"

#include "state_context.h"
#include "state_title.h"
#include "state_character.h"

namespace darwin::state {

    void StateVictory::Enter() {
        logger_->info("Entering victory state");
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
            std::make_unique<modal::ModalVictory>(
                "Victory...",
                modal_victory_params_));
    }

    void StateVictory::Update(StateContext& state_context) {
        if (modal_victory_params_.button_result !=
            modal::ModalVictoryButton::None) {
            switch (modal_victory_params_.button_result) {
            case modal::ModalVictoryButton::Respawn: {
                state_context.ChangeState(
                    std::make_unique<StateCharacter>(
                            app_, 
                            std::move(darwin_client_)));
                }
                    break;
                case modal::ModalVictoryButton::Exit:
                    state_context.ChangeState(
                        std::make_unique<StateTitle>(app_));
                    break;
            }
        }
    }

    void StateVictory::Exit() {
        logger_->info("Exit victory state");
    }

}  // namespace darwin::state.