#include "state_server.h"

#include <memory>

#include "frame/gui/draw_gui_interface.h"
#include "state_context.h"
#include "state_login.h"
#include "state_disconnected.h"
#include "state_title.h"
#include "state_ping.h"

namespace darwin::state {

    void StateServer::Enter() {
        logger_->info("Entering server state");
        for (auto* plugin : app_.GetWindow().GetDevice().GetPluginPtrs()) {
            logger_->info("\tPlugin: [{}] {}", (std::uint64_t)plugin, plugin->GetName().c_str());
            if (!draw_gui_interface_) {
                draw_gui_interface_ =
                    dynamic_cast<frame::gui::DrawGuiInterface*>(
                        plugin);
            }
        }
        if (!draw_gui_interface_) {
            throw std::runtime_error("No draw gui interface plugin found?");
        }
        draw_gui_interface_->AddModalWindow(
            std::make_unique<modal::ModalServer>(
                "Select Server",
                modal_server_params_));
    }

    void StateServer::Update(StateContext& state_context) {
        if (modal_server_params_.button_result != 
            modal::ModalServerButton::None) {
            switch (modal_server_params_.button_result) {
            case modal::ModalServerButton::Connect:
                network_app_ = std::make_unique<NetworkApp>();
                network_app_->EnterWorld(modal_server_params_.server_name);
                state_context.ChangeState(
                    std::make_unique<StatePing>(
                        app_,
                        std::move(network_app_)));
                break;
            case modal::ModalServerButton::Cancel:
                state_context.ChangeState(
                    std::make_unique<StateTitle>(app_));
                break;
            }
        }
    }

    void StateServer::Exit() {
        logger_->info("Exit server state");
    }

} // namespace darwin::state.
