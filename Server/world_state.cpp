#include "world_state.h"

#include <algorithm>
#include <format>
#include <cmath>
#include <assert.h>

#include "Common/darwin_constant.h"
#include "Common/stl_proto_wrapper.h"
#include "Common/vector.h"
#include "Common/convert_math.h"

namespace darwin {

    void WorldState::SetUpgradeElement(std::uint32_t upgrade_count) {
        std::scoped_lock l(mutex_);
        element_max_number_ = upgrade_count;
        AddRandomElementsLocked(upgrade_count);
    }

    bool WorldState::CreateCharacter(
        const std::string& peer,
        const std::string& name,
        const proto::Vector3& color)
    {
        std::scoped_lock l(mutex_);
        auto it = character_infos_.find(name);
        if (it != character_infos_.end()) {
            const auto& character = it->second;
            if (character.status_enum() == proto::STATUS_DEAD) {
                character_infos_.erase(name);
                it = character_infos_.end();
            }
        }
        if (it == character_infos_.end()) {
            proto::Character character;
            character.set_name(name);
            character.mutable_color()->CopyFrom(Normalize(color));
            auto vec3 = CreateRandomNormalizedVector3();
            proto::Physic physic{};
            double radius = 
                GetRadiusFromVolume(player_parameter_.start_mass());
            physic.set_radius(radius);
            physic.set_mass(player_parameter_.start_mass());
            physic.mutable_position()->CopyFrom(
                vec3 * (GetPlanetLocked().physic().radius() + 
                    player_parameter_.drop_height()));
            physic.mutable_position_dt()->CopyFrom(
                CreateVector3(0.0, 0.0, 0.0));
            physic.mutable_orientation()->CopyFrom(
                CreateVector4(0.0, 0.0, 0.0, 1.0));
            physic.mutable_orientation_dt()->CopyFrom(
                CreateVector4(0.0, 0.0, 0.0, 1.0));
            character.mutable_physic()->CopyFrom(physic);
            // WARNING: This suppose the gravity well is at the 
            // position(0, 0, 0).
            character.mutable_normal()->CopyFrom(vec3);
            // This is wrong, and should be set to the real value.
            character.mutable_g_force()->CopyFrom(
                CreateVector3(0.0, 0.0, 0.0));
            character.set_status_enum(proto::STATUS_LOADING);
            character_infos_.insert({ character.name(), character });
            peer_characters_.insert({ peer, name });
            return true;
        }
        else
        {
            std::cerr 
                << std::format(
                    "[{}] Has already a character with name {}.\n",
                    peer,
                    name);
            return false;
        }
    }

    void WorldState::AddCharacter(
        const proto::Character& character) 
    {
        std::scoped_lock l(mutex_);
        if (character_infos_.contains(character.name())) {
            std::cerr 
                << "(test) Error adding character: " 
                << character.name() 
                << "\n";
            return;
        }
        auto it = character_infos_.find(character.name());
        if (it == character_infos_.end()) {
            character_infos_.insert({ character.name(), character });
            // Enter a fake peer to avoid inconsistencies.
            peer_characters_.insert({ character.name(), character.name() });
        }
        else {
            it->second = character;
        }
    }

    proto::Element WorldState::GetPlanet() const {
        std::scoped_lock l(mutex_);
        return GetPlanetLocked();
    }

    proto::Element WorldState::GetPlanetLocked() const {
        for (const auto& [name, element_info] : element_infos_) {
            assert(name == element_info.name());
            if (element_info.type_enum() == proto::TYPE_GROUND) {
                return element_info;
            }
        }
        throw std::runtime_error("No planet found.");
    }

    void WorldState::AddRandomElementsLocked(std::uint32_t number) {
        for (std::uint32_t i = 0; i < number; ++i) {
            proto::Element element;
            static int element_number = 0;
            element.set_name(
                std::format("element_upgrade{}", element_number++));
            element.set_type_enum(proto::TYPE_UPGRADE);
            std::vector<proto::Vector3> colors;
            for (const auto& color : player_parameter_.color_parameters()) {
                colors.push_back(color.color());
            }
            element.mutable_color()->CopyFrom(
                CreateRandomNormalizedColor(colors.begin(), colors.end()));
            auto vec3 = CreateRandomNormalizedVector3();
            proto::Physic physic{};
            double radius = GetRadiusFromVolume(1.0);
            physic.mutable_position()->CopyFrom(
                vec3 * (GetPlanetLocked().physic().radius() + radius));
            physic.mutable_position_dt()->CopyFrom(
                CreateVector3(0.0, 0.0, 0.0));
            physic.set_radius(radius);
            physic.set_mass(1.0);
            element.mutable_physic()->CopyFrom(physic);
            element_infos_.insert({ element.name(), element });
        }
    }

    void WorldState::UpdateCharacter(
        const std::string& name,
        proto::StatusEnum status,
        const proto::Physic& physic)
    {
        std::scoped_lock l(mutex_);
        auto it = character_infos_.find(name);
        if (character_infos_.contains(name)) {
            it->second.mutable_physic()->CopyFrom(physic);
            it->second.set_status_enum(status);
        }
        else {
            std::cerr << "Error updating character: " << name << "\n";
        }
    }

    void WorldState::RemoveCharacter(const std::string& name) {
        std::scoped_lock l(mutex_);
        character_infos_.erase(name);
    }

    bool WorldState::HasCharacter(const std::string& name) const {
        std::scoped_lock l(mutex_);
        return character_infos_.contains(name);
    }

    std::string WorldState::RemovePeer(const std::string& peer) {
        std::scoped_lock l(mutex_);
        return RemovePeerLocked(peer);
    }

    std::string WorldState::RemovePeerLocked(const std::string& peer) {
        if (peer_characters_.contains(peer)) {
            auto character_name = peer_characters_.at(peer);
            peer_characters_.erase(peer);
            character_infos_.erase(character_name);
            return character_name;
        }
        return "";
    }

    std::optional<proto::Character> WorldState::GetCharacterOwnedByPeer(
        const std::string& peer,
        const std::string& character_name) const
    {
        std::scoped_lock l(mutex_);
        if (peer_characters_.contains(peer)) {
            if (character_infos_.contains(peer_characters_.at(peer))) {
                return character_infos_.at(peer_characters_.at(peer));
            }
        }
        return std::nullopt;
    }

    bool WorldState::IsCharacterOwnByPeer(
        const std::string& peer,
        const std::string& character_name) const
    {
        std::scoped_lock l(mutex_);
        if (peer_characters_.contains(peer)) {
            return peer_characters_.at(peer) == character_name;
        }
        return false;
    }

    void WorldState::AddElement(const proto::Element& element) {
        std::scoped_lock l(mutex_);
        auto it = element_infos_.find(element.name());
        if (it == element_infos_.end()) {
            element_infos_.insert({ element.name(), element });
        }
        else {
            it->second = element;
        }
    }

    void WorldState::SetPlayerParameter(
        const proto::PlayerParameter& parameter)
    {
        std::scoped_lock l(mutex_);
        player_parameter_ = parameter;
    }

    void WorldState::CheckIntersectPlayerLocked() {
        std::map<std::string, proto::TypeEnum> to_remove_type;
        for (const auto& [character, target_name] : character_hits_) {
            proto::TypeEnum type_enum = proto::TYPE_UNKNOWN;
            proto::Physic physic_from = character.physic();
            proto::Vector3 color_from = character.color();
            proto::Physic physic_to;
            proto::Vector3 color_to;
            if (element_infos_.contains(target_name)) {
                if (physic_from.mass() > 
                    player_parameter_.max_upgrade_grow()) 
                {
                    // You can't eat any more elements!
                    continue;
                }
                if (element_infos_.at(target_name).type_enum() !=
                    proto::TYPE_UPGRADE)
                {
                    // You can't eat this type of element.
                    continue;
                }
                physic_to = element_infos_.at(target_name).physic();
                color_to = element_infos_.at(target_name).color();
                type_enum = proto::TYPE_UPGRADE;
            }
            if (character_infos_.contains(target_name)) {
                physic_to = character_infos_.at(target_name).physic();
                color_to = character_infos_.at(target_name).color();
                type_enum = proto::TYPE_CHARACTER;
            }
            // Check if you can eat the target.
            if (physic_from.mass() <= physic_to.mass()) {
#ifdef _DEBUG
                std::cerr 
                    << "[" << character.name()
                    << "].mass() <= [" << target_name
                    << "].mass() ?\n";
#endif // _DEBUG
                continue;
            }
#ifdef _DEBUG
            std::cout << std::format(
                "Character {} is trying to eating {} ({}).\n",
                character.name(),
                target_name,
                Dot(
                    Normalize(physic_to.position()), 
                    Normalize(physic_from.position())));
#endif // _DEBUG
            if (IsAlmostIntersecting(physic_from, physic_to)) {
                FromTo from_to{ 
                    character.name(),   target_name,
                    physic_from,        physic_to, 
                    color_from,         color_to
                };
                // Check if color are compatible.
                if (Dot(color_from, color_to) > 0.99) {
                    if (type_enum == proto::TYPE_UPGRADE) {
                        LostSourceElementLocked(from_to);
                    }
                    if (type_enum == proto::TYPE_CHARACTER) {
                        LostSourceCharacterLocked(from_to);
                    }
                }
                else
                {
                    if (type_enum == proto::TYPE_UPGRADE) {
                        ChangeSourceEatUpgradeLocked(from_to);
                        to_remove_type.insert({ target_name, type_enum });
                    }
                    if (type_enum == proto::TYPE_CHARACTER) {
                        ChangeSourceEatCharacterLocked(from_to);
                    }
                }
            }
        }
        for (const auto& [name, type] : to_remove_type) {
            if (type == proto::TYPE_UPGRADE) {
                element_infos_.erase(name);
                AddRandomElementsLocked(1);
            }
            if (type == proto::TYPE_CHARACTER) {
                character_infos_.erase(name);
            }
#ifdef _DEBUG
            if (type == proto::TYPE_UNKNOWN) {
                std::cerr << "[" << name << "]: unknown type.\n";
            }
#endif // _DEBUG
        }
    }

    void WorldState::ChangeSourceEatUpgradeLocked(const FromTo& from_to) {
        proto::Physic physic_from{ from_to.physic_from };
        proto::Physic physic_to{ from_to.physic_to };
        physic_from.set_mass(
            from_to.physic_from.mass() + from_to.physic_to.mass());
        physic_from.set_radius(GetRadiusFromVolume(physic_from.mass()));
        character_infos_.at(
            from_to.name_from).mutable_physic()->CopyFrom(
                physic_from);
    }

    void WorldState::ChangeSourceEatCharacterLocked(const FromTo& from_to) {
        proto::Physic physic_from{ from_to.physic_from };
        proto::Physic physic_to{ from_to.physic_to };
        double move_mass = 
            std::min(physic_to.mass(), player_parameter_.eat_speed());
        physic_from.set_mass(from_to.physic_from.mass() + move_mass);
        physic_from.set_radius(GetRadiusFromVolume(physic_from.mass()));
        character_infos_.at(
            from_to.name_from).mutable_physic()->CopyFrom(
                physic_from);
        physic_to.set_mass(from_to.physic_to.mass() - move_mass);
        physic_to.set_radius(GetRadiusFromVolume(physic_to.mass()));
        character_infos_.at(
            from_to.name_to).mutable_physic()->CopyFrom(
                physic_to);
    }

    void WorldState::LostSourceElementLocked(const FromTo& from_to) {
        proto::Physic physic{ from_to.physic_from };
        physic.set_mass(
            from_to.physic_from.mass() + GetPlayerParameter().penalty());
        physic.set_radius(GetRadiusFromVolume(physic.mass()));
        character_infos_.at(
            from_to.name_from).mutable_physic()->CopyFrom(physic);
    }

    void WorldState::LostSourceCharacterLocked(const FromTo& from_to) {
        double new_mass = 
            (from_to.physic_from.mass() + from_to.physic_to.mass()) * 0.5;
        {
            proto::Physic physic{ from_to.physic_from };
            physic.set_mass(new_mass);
            physic.set_radius(GetRadiusFromVolume(physic.mass()));
            character_infos_.at(
                from_to.name_from).mutable_physic()->CopyFrom(
                    physic);
        }
        {
            proto::Physic physic{ from_to.physic_to };
            physic.set_mass(new_mass);
            physic.set_radius(GetRadiusFromVolume(physic.mass()));
            character_infos_.at(
                from_to.name_to).mutable_physic()->CopyFrom(physic);
        }
    }

    void WorldState::Update(double time) {
        std::scoped_lock l(mutex_);
        if (time != last_updated_) {
            CheckStillInUseCharactersLocked();
            CheckGroundCharactersLocked();
            CheckDeathCharactersLocked();
            CheckVictoryCharactersLocked();
            CheckIntersectPlayerLocked();
            last_updated_ = time;
        }
        FillVectorsLocked();
    }

    void WorldState::UpdatePing(const std::string& name) {
        std::scoped_lock l(mutex_);
        last_seen_[name] = last_updated_;
    }

    void WorldState::CheckStillInUseCharactersLocked() {
        for (const auto& [name, time] : last_seen_) {
            if (time + player_parameter_.disconnection_timeout() < 
                last_updated_) 
            {
                std::cout << std::format(
                    "Character {} has been disconnected.\n", 
                    name);
                character_infos_.erase(name);
                last_seen_.erase(name);
                break;
            }
        }
    }

    void WorldState::CheckGroundCharactersLocked() {
        auto ground = GetPlanetLocked();
        for (auto& [name, character_info] : character_infos_) {
            if (character_info.status_enum() == 
                proto::STATUS_ON_GROUND) 
            {
                assert(name == character_info.name());
                auto position_normal = Normalize(
                    character_info.physic().position());
                character_info.mutable_physic()->
                    mutable_position()->
                    CopyFrom(
                        position_normal *
                        (ground.physic().radius() + 
                         character_info.physic().radius()));
                character_info.mutable_normal()->CopyFrom(
                    position_normal);
            }
        }
    }

    void WorldState::CheckDeathCharactersLocked() {
        for (auto& [name, character_info] : character_infos_) {
            assert(name == character_info.name());
            if (character_info.physic().mass() < 1.0) {
                character_info.set_status_enum(
                    proto::STATUS_DEAD);
                for (const auto& [peer, character] : peer_characters_) {
                    if (character == character_info.name()) {
                        peer_characters_.erase(peer);
                        break;
                    }
                }
            }
        }
    }

    void WorldState::CheckVictoryCharactersLocked() {
        for (auto& [name, character_info] : character_infos_) {
            assert(name == character_info.name());
            if (character_info.physic().mass() >=
                player_parameter_.victory_size()) 
            {
                character_info.set_status_enum(
                    proto::STATUS_DEAD);
                for (const auto& [peer, character] : peer_characters_) {
                    if (character == character_info.name()) {
                        peer_characters_.erase(peer);
                        break;
                    }
                }
            }
        }
    }

    double WorldState::GetLastUpdated() const {
        return last_updated_;
    }

    void WorldState::FillVectorsLocked() {
        characters_.clear();
        elements_.clear();
        for (const auto& [_, character_info] : character_infos_) {
            characters_.push_back(character_info);
        }
        bool first_loop = true;
        for (const auto& [_, element_info] : element_infos_) {
            elements_.push_back(element_info);
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

    void WorldState::SetCharacterHits(
        const std::map<proto::Character, std::string>& character_hits)
    {
        std::scoped_lock l(mutex_);
        character_hits_.clear();
        character_hits_ = character_hits;
    }

}  // End namespace darwin.
