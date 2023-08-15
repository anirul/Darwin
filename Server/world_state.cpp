#include "world_state.h"

namespace darwin {

void WorldState::AddPlayer(double time, const proto::Player& player) {
  auto it = player_infos_.find(player.user_name());
  if (it == player_infos_.end()) {
    PlayerInfo player_info{time, player};
    player_infos_.emplace(player.user_name(), player_info);
  } else {
    it->second.time = time;
    it->second.player = player;
  }
}

void WorldState::AddElement(double time, const proto::Element& element) {
  auto it = element_infos_.find(element.element_name());
  if (it == element_infos_.end()) {
    ElementInfo element_info{time, element};
    element_infos_.emplace(element.element_name(), element_info);
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
  for (auto& name_player_info : player_infos_) {
    auto& player_info = name_player_info.second;
    double delta = time - player_info.time;
    auto new_physic = NewDeltaPhysic(player_info.player.physic(), delta);
    *player_info.player.mutable_physic() = new_physic;
  }
  for (auto& name_element_info : element_infos_) {
    auto& element_info = name_element_info.second;
    double delta = time - element_info.time;
    auto new_physic = NewDeltaPhysic(element_info.element.physic(), delta);
    *element_info.element.mutable_physic() = new_physic;
  }
}

const std::vector<proto::Player>& WorldState::GetPlayers() const {
  return players_;
}

const std::vector<proto::Element>& WorldState::GetElement() const {
  return elements_;
}

proto::Physic WorldState::NewDeltaPhysic(const proto::Physic& physic,
                                         double delta) const {
}

}  // End namespace darwin.
