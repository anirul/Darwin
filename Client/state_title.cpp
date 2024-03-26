#include "state_title.h"

#include <memory>

#include "frame/file/file_system.h"
#include "state_server.h"
#include "state_context.h"
#include "overlay_state.h"

namespace darwin::state {

    StateTitle::StateTitle(
        frame::common::Application& app,
        audio::AudioSystem& audio_system)
        : app_(app),
          audio_system_(audio_system) {}

    void StateTitle::Enter(
        const proto::ClientParameter& client_parameter) 
    {
        logger_->info("Entering title state");
        audio_system_.PlayMusic(proto::AUDIO_MUSIC_TITLE);
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
#ifdef _DEBUG
        auto overlay_state = std::make_unique<overlay::OverlayState>(
            "overlay_state",
            client_parameter_,
            client_parameter_.overlay_state());
        overlay_state->SetStateName("state title");
        draw_gui_interface_->AddOverlayWindow(
            glm::vec2(0.0f, 0.0f),
            app_.GetWindow().GetDevice().GetSize(),
            std::move(overlay_state));
#endif // _DEBUG
        draw_gui_interface_->AddOverlayWindow(
            glm::vec2(0.0f, 0.0f),
            app_.GetWindow().GetDevice().GetSize(),
            std::make_unique<overlay::OverlayTitle>(
                "overlay_title",
                client_parameter_,
                client_parameter_.overlay_title()));
        app_.GetWindow().AddKeyCallback(' ', [this] {
                logger_->info("Space key pressed");
                passed_ = true;
                return true;
            });
        start_time_ = std::chrono::system_clock::now();
    }

    void StateTitle::Exit() {
        logger_->info("Exiting title state");
        app_.GetWindow().RemoveKeyCallback(' ');
        draw_gui_interface_->DeleteWindow("overlay_title");
#ifdef _DEBUG
        draw_gui_interface_->DeleteWindow("overlay_state");
#endif // _DEBUG
    }

    void StateTitle::Update(StateContext& state_context) {
        auto duration = std::chrono::system_clock::now() - start_time_;
        if (duration > std::chrono::seconds(10) && 
            !draw_gui_interface_->IsVisible()) 
        {
            logger_->info("10 seconds passed");
            passed_ = true;
        }
        if (passed_) {
            state_context.ChangeState(
                std::make_unique<StateServer>(app_, audio_system_));
        }
    }

} // namespace darwin::state.
