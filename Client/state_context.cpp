#include "state_context.h"

namespace darwin::state {

StateContext::StateContext(
    std::unique_ptr<StateInterface> state,
    const proto::ClientParameter& client_parameter) 
    : state_(std::move(state)),
      client_parameter_(client_parameter)
{
    state_->Enter(client_parameter_);
} 

void StateContext::ChangeState(std::unique_ptr<StateInterface> state) {
    state_->Exit();
    state_ = std::move(state);
    state_->Enter(client_parameter_);
}

void StateContext::Update() {
    state_->Update(*this);
}

StateContext::~StateContext() {
    state_->Exit();
}

} // namespace darwin::state.
