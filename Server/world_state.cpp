#include "world_state.h"

#include "physic_engine.h"
#include "Common/stl_proto_wrapper.h"

namespace darwin {

    void WorldState::AddCharacter(
        double time, 
        const proto::Character& character) {
        std::scoped_lock l(mutex_info_);
        auto it = character_infos_.find(character.name());
        if (it == character_infos_.end()) {
            CharacterInfo character_info{ time, character };
            character_infos_.emplace(character.name(), character_info);
        }
        else {
            it->second.time = time;
            it->second.character = character;
        }
    }

    void WorldState::UpdateCharacter(
        double time, 
        const std::string& name, 
        const proto::Physic& physic) 
    {
        std::scoped_lock l(mutex_info_);
        auto it = character_infos_.find(name);
        if (it == character_infos_.end()) {
            proto::Character character;
            character.set_name(name);
            *character.mutable_physic() = physic;
            CharacterInfo character_info{ time, character };
            character_infos_.emplace(name, character_info);
        }
        else {
            it->second.time = time;
            *it->second.character.mutable_physic() = physic;
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
        if (time != last_updated_) {
            last_updated_ = time;
            PhysicEngine physic_engine(element_infos_, character_infos_);
            physic_engine.ComputeAllInfo(time);
        }
        FillVectorsLocked();
    }

    const std::vector<proto::Character>& WorldState::GetCharacters() const {
        return characters_;
    }

    const std::vector<proto::Element>& WorldState::GetElements() const {
        return elements_;
    }

    double WorldState::GetLastUpdated() const {
        return last_updated_;
    }

    void WorldState::FillVectorsLocked() {
        characters_.clear();
        elements_.clear();
        for (const auto& [_, character_info] : character_infos_) {
            characters_.push_back(character_info.character);
        }
        for (const auto& [_, element_info] : element_infos_) {
            elements_.push_back(element_info.element);
        }
    }

    bool WorldState::operator==(const WorldState& other) const {
        if (last_updated_ != other.last_updated_) {
            return false;
        }
        if (character_infos_.size() != other.character_infos_.size()) {
            return false;
        }
        if (element_infos_.size() != other.element_infos_.size()) {
            return false;
        }
        for (size_t i = 0; i < characters_.size(); ++i) {
            if (!(characters_[i] == other.characters_[i])) {
                return false;
            }
        }
        for (size_t i = 0; i < elements_.size(); ++i) {
            if (!(elements_[i] == other.elements_[i])) {
                return false;
            }
        }
        return true;
    }

}  // End namespace darwin.
