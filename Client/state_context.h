#pragma once

#include <memory>

#include "state_interface.h"

namespace darwin::state {

    class StateContext {
    public:
        StateContext(std::unique_ptr<StateInterface> state);
        ~StateContext();
        void ChangeState(std::unique_ptr<StateInterface> new_state);
        void Update();

    private:
        std::unique_ptr<StateInterface> state_;
    };

} // namespace darwin::state.
