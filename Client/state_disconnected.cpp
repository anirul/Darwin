#include "state_disconnected.h"

#include <memory>

#include "frame/common/application.h"
#include "frame/gui/draw_gui_interface.h"
#include "modal_disconnected.h"
#include "state_ping.h"
#include "state_context.h"
#include "state_title.h"
#include "overlay_state.h"

namespace darwin::state {

    void StateDisconnected::Enter(
        const proto::ClientParameter& client_parameter) 
    {
        logger_->info("State disconnected entered");
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
        overlay_state->SetStateName("state disconnected");
        draw_gui_->AddOverlayWindow(
            glm::vec2(0.0f, 0.0f),
            app_.GetWindow().GetDevice().GetSize(),
            std::move(overlay_state));
#endif // _DEBUG
        draw_gui_->AddModalWindow(
            std::make_unique<modal::ModalDisconnected>(
                "Disconnected",
                modal_disconnected_params_));
    }

    void StateDisconnected::Update(StateContext& state_context) {
        if (modal_disconnected_params_.button_result !=
            modal::ModalDisconnectedButton::None) {
            switch (modal_disconnected_params_.button_result) {
            case modal::ModalDisconnectedButton::Retry:
                state_context.ChangeState(
                    std::make_unique<StatePing>(
                        app_,
                        std::move(darwin_client_)));
                break;
            case modal::ModalDisconnectedButton::Cancel:
                state_context.ChangeState(
                    std::make_unique<StateTitle>(app_));
                break;
            default:
                throw std::runtime_error(
                    "Unknown button result?");
            }
        }
    }

    void StateDisconnected::Exit() {
        logger_->info("State disconnected exited");
#ifdef _DEBUG
        draw_gui_->DeleteWindow("overlay_state");
#endif // _DEBUG
    }

} // namespace darwin::state.
