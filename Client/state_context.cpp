#include "state_context.h"

namespace darwin::state {

StateContext::StateContext(std::unique_ptr<StateInterface> state) : state_(std::move(state)) {
    state_->Enter();
} 

void StateContext::ChangeState(std::unique_ptr<StateInterface> state) {
    state_->Exit();
    state_ = std::move(state);
    state_->Enter();
}

void StateContext::Update() {
    state_->Update(*this);
}

StateContext::~StateContext() {
    state_->Exit();
}

} // namespace darwin::state.
