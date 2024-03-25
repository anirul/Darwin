#pragma once

#include "state_interface.h"
#include "frame/common/application.h"
#include "frame/logger.h"
#include "modal_disconnected.h"
#include "darwin_client.h"
#include "frame/gui/draw_gui_interface.h"
#include "Common/client_parameter.pb.h"

namespace darwin::state {

    class StateDisconnected : public StateInterface {
    public:
        StateDisconnected(
            frame::common::Application& app, 
            audio::AudioSystem& audio_system,
            std::unique_ptr<DarwinClient> darwin_client)
            : app_(app), 
              audio_system_(audio_system),
              darwin_client_(std::move(darwin_client)) {}
        ~StateDisconnected() override = default;

    public:
        void Enter(const proto::ClientParameter& client_parameter) override;
        void Update(StateContext& state_context) override;
        void Exit() override;

    private:
        frame::common::Application& app_;
        audio::AudioSystem& audio_system_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
        frame::gui::DrawGuiInterface* draw_gui_ = nullptr;
        modal::ModalDisconnectedParams modal_disconnected_params_;
        std::unique_ptr<DarwinClient> darwin_client_;
        proto::ClientParameter client_parameter_;
    };

} // namespace darwin::state.
