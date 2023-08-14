#include "world_state.h"

namespace darwin {

void WorldState::AddPlayer(double time, const proto::Player& player) {}

void WorldState::Update(double time) {}

const std::vector<proto::Player>& WorldState::GetPlayers() const {
    return players_;
}

const std::vector<proto::Element>& WorldState::GetElement() const {
    return elements_;
}

} // End namespace darwin.
