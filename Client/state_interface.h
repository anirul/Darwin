#pragma once

#include "Common/client_parameter.pb.h"

namespace darwin::state {

    class StateContext;

    /**
    * @class StateInterface
    * @brief Interface for state machine design pattern.
    */
    class StateInterface {
    public:
        virtual ~StateInterface() = default;
        //! @brief Enter state.
        virtual void Enter(
            const proto::ClientParameter& client_parameter) = 0;
        //! @brief Update state.
        virtual void Update(StateContext& state_context) = 0;
        //! @brief Exit state.
        virtual void Exit() = 0;
    };

} // namespace darwin::state.
