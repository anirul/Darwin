#include "state_server.h"

#include <memory>

#include "frame/gui/draw_gui_interface.h"
#include "state_context.h"
#include "state_login.h"
#include "state_disconnected.h"
#include "state_title.h"

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
        modal_status_ = ModalStatus::kServer;
        draw_gui_interface_->AddModalWindow(
            std::make_unique<modal::ModalServer>(
                "Select Server", 
                modal_server_params_));
    }

    void StateServer::Update(StateContext& state_context) {
        if (modal_server_params_.succeeded) {
            if (modal_status_ == ModalStatus::kServer) {
                network_app_ = std::make_unique<darwin::NetworkApp>();
                network_app_->EnterWorld(modal_server_params_.server_name);
                modal_status_ = ModalStatus::kPing;
                draw_gui_interface_->AddModalWindow(
                    std::make_unique<modal::ModalPing>(
                        "Connecting to server", 
                        modal_ping_params_, 
                        [this]() {
                            return network_app_->Ping(
                                modal_ping_params_.ping_value);
                        }));
            }
            if (modal_ping_params_.succeeded) {
                if (modal_ping_params_.ping_value != 0) {
                    state_context.ChangeState(
                        std::make_unique<StateLogin>(
                            app_, 
                            std::move(network_app_)));
                }
                else {
                    state_context.ChangeState(
                        std::make_unique<StateTitle>(app_));
                }
            }
        }
        network_app_->Run();
    }

    void StateServer::Exit() {
        modal_status_ = ModalStatus::kServer;
        logger_->info("Exit server state");
    }

} // namespace darwin::state.
