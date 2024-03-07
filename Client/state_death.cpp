#include "state_death.h"

#include "state_context.h"
#include "state_title.h"
#include "state_character.h"

namespace darwin::state {

    void StateDeath::Enter() {
        logger_->info("Entering death state");
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
            std::make_unique<modal::ModalDeath>(
                "Death...",
                modal_death_params_));
    }

    void StateDeath::Update(StateContext& state_context) {
        if (modal_death_params_.button_result !=
            modal::ModalDeathButton::None) {
            switch (modal_death_params_.button_result) {
            case modal::ModalDeathButton::Respawn: {
                state_context.ChangeState(
                    std::make_unique<StateCharacter>(
                            app_, 
                            std::move(darwin_client_)));
                }
                    break;
                case modal::ModalDeathButton::Exit:
                    state_context.ChangeState(
                        std::make_unique<StateTitle>(app_));
                    break;
            }
        }
    }

    void StateDeath::Exit() {
        logger_->info("Exit death state");
    }

} // namespace darwin::state.
