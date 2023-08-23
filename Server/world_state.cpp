#include "world_state.h"

#include "physic_engine.h"

namespace darwin {

    void WorldState::AddPlayer(double time, const proto::Player& player) {
        std::scoped_lock l(mutex_info_);
        auto it = player_infos_.find(player.name());
        if (it == player_infos_.end()) {
            PlayerInfo player_info{ time, player };
            player_infos_.emplace(player.name(), player_info);
        }
        else {
            it->second.time = time;
            it->second.player = player;
        }
    }

    void WorldState::AddElement(double time, const proto::Element& element) {
        std::scoped_lock l(mutex_info_);
        auto it = element_infos_.find(element.name());
        if (it == element_infos_.end()) {
            ElementInfo element_info{ time, element };
            element_infos_.emplace(element.name(), element_info);
        }
        else {
            it->second.time = time;
            it->second.element = element;
        }
    }

    void WorldState::Update(double time) {
        std::scoped_lock l(mutex_info_);
        if (time == last_updated_) {
            std::cerr 
                << "Warning try to re-update the world state." 
                << std::endl;
            return;
        }
        last_updated_ = time;
        PhysicEngine physic_engine(element_infos_, player_infos_);
        physic_engine.ComputeAllInfo(time);
        FillVectorsLocked();
    }

    const std::vector<proto::Player>& WorldState::GetPlayers() const {
        return players_;
    }

    const std::vector<proto::Element>& WorldState::GetElements() const {
        return elements_;
    }

    void WorldState::FillVectorsLocked() {
        players_.clear();
        elements_.clear();
        for (const auto& [_, player_info] : player_infos_) {
            players_.push_back(player_info.player);
        }
        for (const auto& [_, element_info] : element_infos_) {
            elements_.push_back(element_info.element);
        }
    }

}  // End namespace darwin.
