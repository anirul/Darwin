#pragma once

#include "state_interface.h"
#include "frame/common/application.h"
#include "frame/gui/draw_gui_interface.h"
#include "frame/logger.h"
#include "modal_server.h"
#include "modal_ping.h"
#include "darwin_client.h"
#include "Common/client_parameter.pb.h"

namespace darwin::state {

    class StateContext;

    /**
     * @class StateInterface
     * @brief Interface for state machine design pattern.
     */
    class StateServer : public StateInterface {
    public:
        StateServer(frame::common::Application& app) : app_(app) {}
        ~StateServer() = default;
        //! @brief Enter state.
        void Enter(const proto::ClientParameter& client_parameter) override;
        //! @brief Update state.
        void Update(StateContext& state_context) override;
        //! @brief Exit state.
        void Exit() override;

    private:
        frame::common::Application& app_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
        frame::gui::DrawGuiInterface* draw_gui_interface_ = nullptr;
        modal::ModalServerParams modal_server_params_;
        std::unique_ptr<DarwinClient> darwin_client_ = nullptr;
        proto::ClientParameter client_parameter_;
    };

} // namespace darwin::state.
