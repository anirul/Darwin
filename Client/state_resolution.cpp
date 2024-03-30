#include "state_resolution.h"

#include <memory>

namespace darwin::state {
    
    StateResolution::StateResolution(
        frame::common::Application& app,
        audio::AudioSystem& audio_system) :
        app_(app),
        audio_system_(audio_system) 
    {
    }
    
    void StateResolution::Enter(
        const proto::ClientParameter& client_parameter) 
    {
        logger_->info("Entering resolution state");
        client_parameter_ = client_parameter;
        for (auto* plugin : app_.GetWindow().GetDevice().GetPluginPtrs()) {
            logger_->info(
                "\tPlugin: [{}] {}",
                (std::uint64_t)plugin,
                plugin->GetName().c_str());
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
            std::make_unique<modal::ModalResolution>(
                "Select base resolution",
                modal_resolution_params_));
    }

    void StateResolution::Update(StateContext& state_context) {
        if (modal_resolution_params_.button_result !=
            modal::ModalResolutionButton::None) 
        {
            switch (modal_resolution_params_.button_result) {
            case modal::ModalResolution::Change:
                app_.Resize(
                    modal_resolution_params_.resolution,
                    modal_resolution_params_.fullscreen_enum);
                break;
            case modal::ModalResolution::Start:
                state_context.ChangeState(
                    std::make_unique<StateTitle>(app_, audio_system_));
                break;
            }
        }
    }

    void StateResolution::Exit() {
        logger_->info("Exiting resolution state");
    }

} // namespace darwin::state.