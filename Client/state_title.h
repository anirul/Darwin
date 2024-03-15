#pragma once

#include "frame/logger.h"
#include "state_interface.h"
#include "frame/common/application.h"
#include "frame/gui/draw_gui_interface.h"

namespace darwin::state {

    class StateTitle : public StateInterface {
    public:
        StateTitle(frame::common::Application& app);
        ~StateTitle() override = default;
        void Enter() override;
        void Update(StateContext& state_context) override;
        void Exit() override;

    private:
        frame::Logger& logger_ = frame::Logger::GetInstance();
        frame::common::Application& app_;
        std::chrono::time_point<std::chrono::system_clock> start_time_;
        frame::gui::DrawGuiInterface* draw_gui_interface_ = nullptr;
        bool passed_ = false;
    };

} // namespace darwin::state.
