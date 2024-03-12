#include "state_title.h"

#include <memory>

#include "frame/file/file_system.h"
#include "state_server.h"
#include "state_context.h"

namespace darwin::state {

    StateTitle::StateTitle(frame::common::Application& app) : app_(app) {}

    void StateTitle::Enter() {
        logger_->info("Entering title state");
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
                std::make_unique<StateServer>(app_));
        }
    }

} // namespace darwin::state.
