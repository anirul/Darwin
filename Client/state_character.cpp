#include "state_character.h"

#include "state_context.h"
#include "state_play.h"
#include "state_server.h"
#include "overlay_state.h"

namespace darwin::state {

    void StateCharacter::Enter(
        const proto::ClientParameter& client_parameter) 
    {
        logger_->info("Entering character state");
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
        overlay_state->SetStateName("state character");
        draw_gui_->AddOverlayWindow(
            glm::vec2(0.0f, 0.0f),
            app_.GetWindow().GetDevice().GetSize(),
            std::move(overlay_state));
#endif // _DEBUG
        const std::vector<proto::ColorParameter> colors =
            darwin_client_->GetColorParameters();
        draw_gui_->AddModalWindow(
            std::make_unique<modal::ModalCharacter>(
                "Select Character",
                modal_character_params_,
                colors));
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
                    }
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
#ifdef _DEBUG
        draw_gui_->DeleteWindow("overlay_state");
#endif // _DEBUG
    }

} // namespace darwin::state.
