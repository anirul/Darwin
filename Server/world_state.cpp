#include "world_state.h"
#include "physic_engine.h"

namespace darwin {

void WorldState::AddPlayer(double time, const proto::Player& player) {
  auto it = player_infos_.find(player.name());
  if (it == player_infos_.end()) {
    PlayerInfo player_info{time, player};
    player_infos_.emplace(player.name(), player_info);
  } else {
    it->second.time = time;
    it->second.player = player;
  }
}

void WorldState::AddElement(double time, const proto::Element& element) {
  auto it = element_infos_.find(element.name());
  if (it == element_infos_.end()) {
    ElementInfo element_info{time, element};
    element_infos_.emplace(element.name(), element_info);
  } else {
    it->second.time = time;
    it->second.element = element;
  }
}

void WorldState::Update(double time) {
  if (time == last_updated_) {
    std::cerr << "Warning try to re-update the world state." << std::endl;
    return;
  }
  last_updated_ = time;
  PhysicEngine physic_engine(element_infos_, player_infos_);
  physic_engine.ComputeElementInfo(time);
  physic_engine.ComputePlayerInfo(time);
}

const std::vector<proto::Player>& WorldState::GetPlayers() const {
  return players_;
}

const std::vector<proto::Element>& WorldState::GetElement() const {
  return elements_;
}

}  // End namespace darwin.
