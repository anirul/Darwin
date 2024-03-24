#pragma once

#include "state_interface.h"
#include "darwin_client.h"
#include "frame/common/application.h"
#include "frame/logger.h"
#include "frame/gui/draw_gui_interface.h"
#include "modal_victory.h"
#include "Common/client_parameter.pb.h"

namespace darwin::state {

    class StateVictory : public StateInterface {
    public:
        StateVictory(
            frame::common::Application& app,
            std::unique_ptr<darwin::DarwinClient> darwin_client) :
            app_(app), darwin_client_(std::move(darwin_client)) {}
        ~StateVictory() override = default;

        void Enter(const proto::ClientParameter& client_parameter) override;
        void Update(StateContext& state_context) override;
        void Exit() override;

    private:
        frame::common::Application& app_;
        std::unique_ptr<darwin::DarwinClient> darwin_client_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
        frame::gui::DrawGuiInterface* draw_gui_ = nullptr;
        darwin::modal::ModalVictoryParams modal_victory_params_;
        proto::ClientParameter client_parameter_;
    };

}  // namespace darwin::state.
