#include "state_victory.h"

#include "state_context.h"
#include "state_title.h"
#include "state_character.h"
#include "overlay_state.h"

namespace darwin::state {

    void StateVictory::Enter(const proto::ClientParameter& client_parameter) {
        logger_->info("Entering victory state");
        audio_system_.PlayMusic(proto::AUDIO_MUSIC_WIN);
        client_parameter_ = client_parameter;
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
#ifdef _DEBUG
        auto overlay_state = std::make_unique<overlay::OverlayState>(
            "overlay_state",
            client_parameter_,
            client_parameter_.overlay_state());
        overlay_state->SetStateName("state victory");
        draw_gui_->AddOverlayWindow(
            glm::vec2(0.0f, 0.0f),
            app_.GetWindow().GetDevice().GetSize(),
            std::move(overlay_state));
#endif // _DEBUG
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
                        audio_system_,
                        std::move(darwin_client_)));
                }
                    break;
                case modal::ModalVictoryButton::Exit:
                    state_context.ChangeState(
                        std::make_unique<StateTitle>(app_, audio_system_));
                    break;
            }
        }
    }

    void StateVictory::Exit() {
        logger_->info("Exit victory state");
#ifdef _DEBUG
        draw_gui_->DeleteWindow("overlay_state");
#endif // _DEBUG
    }

}  // namespace darwin::state.