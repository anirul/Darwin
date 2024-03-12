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
        std::scoped_lock l(mutex_);
        auto it = character_infos_.find(name);
        if (it != character_infos_.end()) {
            const auto& character = it->second.character;
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
            CharacterInfo character_info{ GetLastUpdated(), character};
            character_infos_.emplace(character.name(), character_info);
            peer_characters_.emplace(peer, name);
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
        double time, 
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
        std::scoped_lock l(mutex_);
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
        std::scoped_lock l(mutex_);
        for (std::uint32_t i = 0; i < number; ++i) {
            proto::Element element;
            element.set_name(std::format("element{}", i));
            element.set_type_enum(proto::TYPE_UPGRADE);
            std::vector<proto::Vector3> colors;
            for (const auto& color : player_parameter_.colors()) {
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
            ElementInfo element_info{ GetLastUpdated(), element };
            element_infos_.emplace(element.name(), element_info);
        }
    }

    void WorldState::UpdateCharacter(
        double time,
        const std::string& name,
        proto::StatusEnum status,
        const proto::Physic& physic)
    {
        std::scoped_lock l(mutex_);
        auto it = character_infos_.find(name);
        if (character_infos_.contains(name)) {
            it->second.time = time;
            it->second.character.mutable_physic()->CopyFrom(physic);
            it->second.character.set_status_enum(status);
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
            return character_infos_.at(peer_characters_.at(peer)).character;
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

    void WorldState::AddElement(double time, const proto::Element& element) {
        std::scoped_lock l(mutex_);
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
                physic_to = element_infos_.at(target_name).element.physic();
                color_to = element_infos_.at(target_name).element.color();
                type_enum = proto::TYPE_UPGRADE;
            }
            if (character_infos_.contains(target_name)) {
                physic_to = character_infos_.at(
                    target_name).character.physic();
                color_to = character_infos_.at(target_name).character.color();
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
                    ChangeSourceEatLocked(from_to);
                    to_remove_type.insert({ target_name, type_enum });
                }
            }
        }
        for (const auto& [name, type] : to_remove_type) {
            if (type == proto::TYPE_UPGRADE) {
                element_infos_.erase(name);
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

    void WorldState::ChangeSourceEatLocked(const FromTo& from_to) {
        proto::Physic physic{ from_to.physic_from };
        physic.set_mass(
            from_to.physic_from.mass() + from_to.physic_to.mass());
        physic.set_radius(GetRadiusFromVolume(physic.mass()));
        character_infos_.at(
            from_to.name_from).character.mutable_physic()->CopyFrom(physic);
        if (GetPlayerParameter().change_color() == proto::COLOR_YES) {
            auto final_color =
                Normalize(
                    from_to.color_from * from_to.physic_from.mass() +
                    from_to.color_to * from_to.physic_to.mass());
            character_infos_.at(
                from_to.name_from).character.mutable_color()->CopyFrom(
                    final_color);
        }
    }

    void WorldState::LostSourceElementLocked(const FromTo& from_to) {
        proto::Physic physic{ from_to.physic_from };
        physic.set_mass(
            from_to.physic_from.mass() + GetPlayerParameter().penalty());
        physic.set_radius(GetRadiusFromVolume(physic.mass()));
        character_infos_.at(
            from_to.name_from).character.mutable_physic()->CopyFrom(physic);
    }

    void WorldState::LostSourceCharacterLocked(const FromTo& from_to) {
        double new_mass = 
            (from_to.physic_from.mass() + from_to.physic_to.mass()) * 0.5;
        {
            proto::Physic physic{ from_to.physic_from };
            physic.set_mass(new_mass);
            physic.set_radius(GetRadiusFromVolume(physic.mass()));
            character_infos_.at(
                from_to.name_from).character.mutable_physic()->CopyFrom(
                    physic);
        }
        {
            proto::Physic physic{ from_to.physic_to };
            physic.set_mass(new_mass);
            physic.set_radius(GetRadiusFromVolume(physic.mass()));
            character_infos_.at(
                from_to.name_to).character.mutable_physic()->CopyFrom(physic);
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
        for (auto& [_, character_info] : character_infos_) {
            if (character_info.character.status_enum() == 
                proto::STATUS_ON_GROUND) 
            {
                character_info.character.mutable_physic()->
                    mutable_position()->
                    CopyFrom(
                        character_info.character.normal() * 
                        (ground.physic().radius() + 
                         character_info.character.physic().radius()));
            }
        }
    }

    void WorldState::CheckDeathCharactersLocked() {
        for (auto& [_, character_info] : character_infos_) {
            if (character_info.character.physic().mass() < 1.0) {
                character_info.character.set_status_enum(
                    proto::STATUS_DEAD);
                for (const auto& [peer, character] : peer_characters_) {
                    if (character == character_info.character.name()) {
                        peer_characters_.erase(peer);
                        break;
                    }
                }
            }
        }
    }

    void WorldState::CheckVictoryCharactersLocked() {
        for (auto& [_, character_info] : character_infos_) {
            if (character_info.character.physic().mass() >=
                player_parameter_.victory_size()) 
            {
                character_info.character.set_status_enum(
                    proto::STATUS_DEAD);
                for (const auto& [peer, character] : peer_characters_) {
                    if (character == character_info.character.name()) {
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

    void WorldState::SetCharacterHits(
        const std::map<proto::Character, std::string>& character_hits)
    {
        std::scoped_lock l(mutex_);
        character_hits_.clear();
        character_hits_ = character_hits;
    }

}  // End namespace darwin.
