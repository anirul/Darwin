#pragma once

#include "state_interface.h"
#include "darwin_client.h"
#include "frame/common/application.h"
#include "frame/logger.h"
#include "frame/gui/draw_gui_interface.h"
#include "modal_death.h"

namespace darwin::state {

    class StateDeath : public StateInterface {
    public:
        StateDeath(
            frame::common::Application& app,
            std::unique_ptr<darwin::DarwinClient> darwin_client) :
            app_(app), darwin_client_(std::move(darwin_client)) {}
        ~StateDeath() override = default;

        void Enter() override;
        void Update(StateContext& state_context) override;
        void Exit() override;

    private:
        frame::common::Application& app_;
        std::unique_ptr<darwin::DarwinClient> darwin_client_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
        frame::gui::DrawGuiInterface* draw_gui_ = nullptr;
        darwin::modal::ModalDeathParams modal_death_params_;
    };

} // namespace darwin.
