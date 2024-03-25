#pragma once

#include <memory>

#include "frame/logger.h"
#include "state_interface.h"
#include "overlay_title.h"
#include "frame/common/application.h"
#include "frame/gui/draw_gui_interface.h"
#include "Common/client_parameter.pb.h"

namespace darwin::state {

    class StateTitle : public StateInterface {
    public:
        StateTitle(
            frame::common::Application& app,
            audio::AudioSystem& audio_system);
        ~StateTitle() override = default;

    public:
        void Enter(const proto::ClientParameter& client_parameter) override;
        void Update(StateContext& state_context) override;
        void Exit() override;

    private:
        frame::Logger& logger_ = frame::Logger::GetInstance();
        frame::common::Application& app_;
        audio::AudioSystem& audio_system_;
        proto::ClientParameter client_parameter_;
        std::chrono::time_point<std::chrono::system_clock> start_time_;
        frame::gui::DrawGuiInterface* draw_gui_interface_ = nullptr;
        bool passed_ = false;
    };

} // namespace darwin::state.
