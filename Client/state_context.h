#pragma once

#include <memory>

#include "state_interface.h"

namespace darwin::state {

    /**
     * @class StateContext
     * @brief The StateContext class is a context for the state pattern.
     * 
     * States are as follows:
     * - StateTitle (initial state) 
     *      goto StateServer
     * - StateDisconnect (you are disconnected)
     *      goto StateTitle
     * - StateServer (you select a server) 
     *      goto StatePing or StateTitle or StateDisconnect
     * - StatePing (you ping a server)
     *      goto StateCharacter or StateDisconnect
     * - StateCharacter (you select a character) 
     *      goto StatePlay or StateTitle or StateDisconnect
     * - StatePlay (you enter the world) 
     *      goto StateDisconnect or StateCharacter or StateDead
     * - StateDead (you are dead)
     *      goto StateCharacter or StateTitle or StateDisconnect
     */
    class StateContext {
    public:
        StateContext(
            std::unique_ptr<StateInterface> state,
            const proto::ClientParameter& client_parameter);
        ~StateContext();
        void ChangeState(std::unique_ptr<StateInterface> new_state);
        void Update();

    private:
        std::unique_ptr<StateInterface> state_;
        proto::ClientParameter client_parameter_;
    };

} // namespace darwin::state.
