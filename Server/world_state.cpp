#include "world_state.h"

#include <format>
#include <cmath>

#include "Common/darwin_constant.h"
#include "Common/stl_proto_wrapper.h"
#include "Common/vector.h"
#include "Common/convert_math.h"

namespace darwin {

    bool WorldState::CreateCharacter(
        const std::string& peer,
        const std::string& name,
        const proto::Vector3& color)
    {
        std::scoped_lock l(mutex_info_);
        auto it = character_infos_.find(name);
        if (it == character_infos_.end()) {
            proto::Character character;
            character.set_name(name);
            character.mutable_color()->CopyFrom(color);
            auto vec3 = CreateRandomNormalizedVector3();
            proto::Physic physic{};
            double radius = 
                GetRadiusFromVolume(player_parameter_.start_mass());
            physic.set_radius(radius);
            physic.set_mass(player_parameter_.start_mass());
            physic.mutable_position()->CopyFrom(
                MultiplyVector3ByScalar(
                    vec3, 
                    GetPlanetLocked().physic().radius() + 
                    player_parameter_.drop_height()));
            physic.mutable_position_dt()->CopyFrom(
                CreateBasicVector3(0.0, 0.0, 0.0));
            physic.mutable_orientation()->CopyFrom(
                CreateBasicVector4(0.0, 0.0, 0.0, 1.0));
            physic.mutable_orientation_dt()->CopyFrom(
                CreateBasicVector4(0.0, 0.0, 0.0, 1.0));
            character.mutable_physic()->CopyFrom(physic);
            // WARNING: This suppose the gravity well is at the position 
            // (0, 0, 0).
            character.mutable_normal()->CopyFrom(vec3);
            // This is wrong, and should be set to the real value.
            character.mutable_g_force()->CopyFrom(
                CreateBasicVector3(0.0, 0.0, 0.0));
            CharacterInfo character_info{ GetLastUpdated(), character};
            character_infos_.emplace(character.name(), character_info);
            peer_characters_.emplace(peer, name);
            return true;
        }
        else {
            std::cerr 
                << std::format(
                    "[{}] Has already a character with name {}.\n",
                    peer,
                    name);
            return false;
        }
    }

    void WorldState::AddCharacter(
        double time, 
        const proto::Character& character) 
    {
        std::scoped_lock l(mutex_info_);
        if (character_infos_.contains(character.name())) {
            std::cerr 
                << "(test) Error adding character: " 
                << character.name() 
                << "\n";
            return;
        }
        auto it = character_infos_.find(character.name());
        if (it == character_infos_.end()) {
            CharacterInfo character_info{ time, character };
            character_infos_.emplace(character.name(), character_info);
            // Enter a fake peer to avoid inconsistencies.
            peer_characters_.emplace(character.name(), character.name());
        }
        else {
            it->second.time = time;
            it->second.character = character;
        }
    }

    proto::Element WorldState::GetPlanet() const {
        std::scoped_lock l(mutex_info_);
        return GetPlanetLocked();
    }

    proto::Element WorldState::GetPlanetLocked() const {
        for (const auto& [name, element_info] : element_infos_) {
            if (element_info.element.type_enum() == proto::TYPE_GROUND) {
                return element_info.element;
            }
        }
        throw std::runtime_error("No planet found.");
    }

    void WorldState::AddRandomElements(std::uint32_t number) {
        std::scoped_lock l(mutex_info_);
        for (std::uint32_t i = 0; i < number; ++i) {
            proto::Element element;
            element.set_name(std::format("element{}", i));
            element.set_type_enum(proto::TYPE_UPGRADE);
            element.mutable_color()->CopyFrom(
                CreateRandomNormalizedColor());
            auto vec3 = CreateRandomNormalizedVector3();
            proto::Physic physic{};
            double radius = GetRadiusFromVolume(1.0);
            physic.mutable_position()->CopyFrom(
                MultiplyVector3ByScalar(
                    vec3, 
                    GetPlanetLocked().physic().radius() + radius));
            physic.mutable_position_dt()->CopyFrom(
                CreateBasicVector3(0.0, 0.0, 0.0));
            physic.set_radius(radius);
            physic.set_mass(1.0);
            element.mutable_physic()->CopyFrom(physic);
            ElementInfo element_info{ GetLastUpdated(), element };
            element_infos_.emplace(element.name(), element_info);
        }
    }

    void WorldState::UpdateCharacter(
        double time,
        const std::string& name,
        const proto::Physic& physic)
    {
        std::scoped_lock l(mutex_info_);
        auto it = character_infos_.find(name);
        if (character_infos_.contains(name)) {
            it->second.time = time;
            *it->second.character.mutable_physic() = physic;
        }
        else {
            std::cerr << "Error updating character: " << name << "\n";
        }
    }

    void WorldState::RemoveCharacter(const std::string& name) {
        std::scoped_lock l(mutex_info_);
        character_infos_.erase(name);
    }

    bool WorldState::HasCharacter(const std::string& name) const {
        std::scoped_lock l(mutex_info_);
        return character_infos_.contains(name);
    }

    std::string WorldState::RemovePeer(const std::string& peer) {
        std::scoped_lock l(mutex_info_);
        if (peer_characters_.contains(peer)) {
            auto character_name = peer_characters_.at(peer);
            peer_characters_.erase(peer);
            character_infos_.erase(character_name);
            return character_name;
        }
        return "";
    }

    bool WorldState::IsCharacterOwnByPeer(
        const std::string& peer,
        const std::string& character_name) const
    {
        std::scoped_lock l(mutex_info_);
        if (peer_characters_.contains(peer)) {
            return peer_characters_.at(peer) == character_name;
        }
        return false;
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

    void WorldState::SetPlayerParameter(
        const proto::PlayerParameter& parameter)
    {
        std::scoped_lock l(mutex_info_);
        player_parameter_ = parameter;
    }

    void WorldState::CheckIntersectPlayerLocked() {
        std::vector<std::string> to_remove;
        proto::TypeEnum type_enum = proto::TYPE_UNKNOWN;
        for (const auto& [name, target] : potential_hits_) {
            proto::Physic physic_from = 
                character_infos_.at(name).character.physic();
            proto::Vector3 color_from = 
                character_infos_.at(name).character.color();
            proto::Physic physic_to;
            proto::Vector3 color_to;
            if (element_infos_.contains(target)) {
                physic_to = element_infos_.at(target).element.physic();
                color_to = element_infos_.at(target).element.color();
                type_enum = proto::TYPE_UPGRADE;
            }
            if (character_infos_.contains(target)) {
                physic_to = character_infos_.at(target).character.physic();
                color_to = character_infos_.at(target).character.color();
                type_enum = proto::TYPE_CHARACTER;
            }
            // Check if you can eat the target.
            if (physic_from.mass() <= physic_to.mass()) {
#ifdef _DEBUG
                std::cerr 
                    << "[" << name 
                    << "].mass() <= [" << target 
                    << "].mass() ?\n";
#endif // _DEBUG
                continue;
            }
            if (IsIntersecting(physic_from, physic_to)) {
                // Check if color are compatible.
                if (DotProduct(color_from, color_to) > 0.9) {

                } else {
                    FromTo from_to{
                        name,
                        target,
                        physic_from,
                        physic_to,
                        color_from,
                        color_to
                    };
                    if (DotProduct(color_from, color_to) > 0.95) 
                    {
                        if (type_enum == proto::TYPE_UPGRADE) {
                            LostSourceElementLocked(from_to);
                        }
                        if (type_enum == proto::TYPE_CHARACTER) {
                            LostSourceCharacterLocked(from_to);
                        }
                    }
                    else
                    {
                        ChangeSourceEatLocked(from_to);
                        to_remove.push_back(target);
                    }
                }
            }
        }
        for (const auto& name : to_remove) {
            if (type_enum == proto::TYPE_UPGRADE) {
                element_infos_.erase(name);
            }
            if (type_enum == proto::TYPE_CHARACTER) {
                character_infos_.erase(name);
            }
#ifdef _DEBUG
            if (type_enum == proto::TYPE_UNKNOWN) {
                std::cerr << "[" << name << "]: unknown type.\n";
            }
#endif // _DEBUG
        }
    }

    void WorldState::ChangeSourceEatLocked(const FromTo& from_to) {
        proto::Physic physic{ from_to.physic_from };
        physic.set_mass(
            from_to.physic_from.mass() + from_to.physic_to.mass());
        physic.set_radius(GetRadiusFromVolume(physic.mass()));
        character_infos_.at(
            from_to.name_from).character.mutable_physic()->CopyFrom(physic);
        auto final_color =
            Normalize(
                Add(
                    MultiplyVector3ByScalar(
                        from_to.color_from,
                        from_to.physic_from.mass()),
                    MultiplyVector3ByScalar(
                        from_to.color_to,
                        from_to.physic_to.mass())));
        character_infos_.at(
            from_to.name_from).character.mutable_color()->CopyFrom(
                final_color);
    }

    void WorldState::LostSourceElementLocked(const FromTo& from_to) {
        proto::Physic physic{ from_to.physic_from };

        physic.set_mass(from_to.physic_from.mass() - 0.2);
        character_infos_.at(
            from_to.name_from).character.mutable_physic()->CopyFrom(physic);
    }

    void WorldState::LostSourceCharacterLocked(const FromTo& from_to) {
        double new_mass = 
            (from_to.physic_from.mass() + from_to.physic_to.mass()) * 0.5;
        {
            proto::Physic physic{ from_to.physic_from };
            physic.set_mass(new_mass);
            character_infos_.at(
                from_to.name_from).character.mutable_physic()->CopyFrom(
                    physic);
        }
        {
            proto::Physic physic{ from_to.physic_to };
            physic.set_mass(new_mass);
            character_infos_.at(
                from_to.name_to).character.mutable_physic()->CopyFrom(physic);
        }
    }

    void WorldState::Update(double time) {
        std::scoped_lock l(mutex_info_);
        if (time != last_updated_) {
            CheckIntersectPlayerLocked();
            last_updated_ = time;
        }
        FillVectorsLocked();
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
        bool first_loop = true;
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

    void WorldState::SetPotentialHits(
        const std::map<std::string, std::string>& potential_hits)
    {
        std::scoped_lock l(mutex_info_);
        potential_hits_.clear();
        potential_hits_ = potential_hits;
    }

}  // End namespace darwin.
