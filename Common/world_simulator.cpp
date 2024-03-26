#include "world_simulator.h"

#include <format>

#include "vector.h"
#include "physic.h"
#include "convert_math.h"

namespace darwin {

    void WorldSimulator::SetUserName(const std::string& name) {
        name_ = name;
    }

    std::string WorldSimulator::GetUserName() const {
        return name_;
    }

    void WorldSimulator::UpdateData(
        const std::vector<proto::Element>& elements,
        const std::vector<proto::Character>& characters,
        double time)
    {
        std::lock_guard l(mutex_);
        elements_ = elements;
        characters_ = characters;
        time_ = time;
        started_ = true;
        last_server_update_time_ =
            std::chrono::duration<double>(
                std::chrono::system_clock::now().time_since_epoch()).count();
    }

    std::vector<proto::Element> WorldSimulator::GetGForceElementsLocked() {
        std::vector<proto::Element> result;
        for (auto& element : elements_) {
            if (element.type_enum() == proto::TYPE_GROUND) {
                result.push_back(element);
            }
        }
        return result;
    }

    void WorldSimulator::ApplyGForceAndSpeedToCharacterLocked(
        const std::vector<proto::Element>& static_elements,
        double delta_time)
    {
        if ((delta_time < 0.0001) || (delta_time > 1.0)) return;
        // Apply it to characters.
        for (auto& character : characters_) {
            // TODO(anirul): Temporary hack.
            if (character.name() != name_) continue;
            glm::dvec3 force = glm::dvec3(0.0);
            // Add all gravity forces.
            for (const auto& element : static_elements) {
                auto result = ApplyPhysic(
                    element.physic(),
                    character.physic());
                force = force + result;
            }
            // Update the g part of the character.
            character.mutable_g_force()->CopyFrom(
                Glm2ProtoVector(force));
            character.mutable_normal()->CopyFrom(
                Glm2ProtoVector(glm::normalize(-force)));
            // Update the physic part of the character.
            UpdateObject(
                *character.mutable_physic(),
                force,
                delta_time);
            // Correct the surface.
            for (auto& element : static_elements) {
                auto status_result = CorrectSurface(
                    *character.mutable_physic(),
                    element);
                if (status_result != character.status_enum()) {
                    character.set_status_enum(status_result);
                }
            }
        }
    }

    void WorldSimulator::UpdateTime() {
        std::lock_guard l(mutex_);
        if (!started_) return;
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = now - last_time_;
        double elapsed_seconds =
            std::chrono::duration<double>(elapsed).count();
        time_ += elapsed_seconds;
        last_time_ = now;
        // Get gravity forces.
        std::vector<proto::Element> static_elements = 
            GetGForceElementsLocked();
        // Apply gravity forces to characters.
        ApplyGForceAndSpeedToCharacterLocked(static_elements, elapsed_seconds);
    }

    UniformEnum WorldSimulator::GetUniforms() const {
        std::lock_guard l(mutex_);
        UniformEnum uniform_enum;
        for (const auto& element : elements_) {
            uniform_enum.spheres.push_back(GetSphere(element.physic()));
            uniform_enum.colors.push_back(GetColor(element));
        }
        for (const auto& character : characters_) {
            uniform_enum.spheres.push_back(GetSphere(character.physic()));
            uniform_enum.colors.push_back(GetColor(character));
        }
        return uniform_enum;
    }

    bool WorldSimulator::HasCharacter(const std::string& name) const {
        std::lock_guard l(mutex_);
        for (const auto& character : characters_) {
            if (character.name() == name) {
                return true;
            }
        }
        return false;
    }

    SoundEffectEnum WorldSimulator::GetSoundEffect(
        const std::string& player_character_name) 
    {
        std::lock_guard l(mutex_);
        if (player_character_.name() != player_character_name) {
            for (const auto& character : characters_) {
                if (character.name() == player_character_name) {
                    player_character_ = character;
                    return SoundEffectEnum::SOUND_EFFECT_NONE;
                }
            }
        }
        for (const auto& character : characters_) {
            if (character.name() == player_character_.name()) {
                if (character.physic().mass() > 
                    player_character_.physic().mass()) 
                {
                    player_character_ = character;
                    return SoundEffectEnum::SOUND_EFFECT_GOOD;
                }
                if (character.physic().mass() + 0.1 < 
                    player_character_.physic().mass()) 
                {
                    player_character_ = character;
                    return SoundEffectEnum::SOUND_EFFECT_BAD;
                }
                player_character_ = character;
            }
        }
        return SoundEffectEnum::SOUND_EFFECT_NONE;
    }

    bool WorldSimulator::IsClose(
        const proto::Vector3& normal,
        const proto::Vector3& position) const
    {
        return Dot(normal, position) > 0.8;
    }

    UniformEnum WorldSimulator::GetCloseUniforms(
        const proto::Vector3& normal,
        double delta_time) const
    {
        std::lock_guard l(mutex_);
        UniformEnum uniform_enum;
        for (const auto& element : elements_) {
            // Check if this is the planet (should have a radius > 50.0) or
            // if it is close enough to the normal.
            if ((element.physic().radius() > 50.0) || 
                (IsClose(normal, Normalize(element.physic().position())))) {
                uniform_enum.spheres.push_back(GetSphere(element.physic()));
                uniform_enum.colors.push_back(GetColor(element));
            }
        }
        for (const auto& character : characters_) {
            if (character.status_enum() == proto::STATUS_DEAD) continue;
            if (IsClose(normal, Normalize(character.physic().position()))) {
                if (character.name() == GetUserName()) {
                    uniform_enum.spheres.push_back(
                        GetSphere(character.physic()));
                }
                else {
                    uniform_enum.spheres.push_back(
                        GetSphereUdpate(character.physic(), delta_time));
                }
                uniform_enum.colors.push_back(GetColor(character));
            }
        }
        return uniform_enum;
    }

    glm::vec4 WorldSimulator::GetSphere(const proto::Physic& physic) const {
        return glm::vec4(
            physic.position().x(),
            physic.position().y(),
            physic.position().z(),
            physic.radius());
    }

    glm::vec4 WorldSimulator::GetSphereUdpate(
        const proto::Physic& physic, 
        double delta_time) const
    {
        return glm::vec4(
            physic.position().x() + physic.position_dt().x() * delta_time,
            physic.position().y() + physic.position_dt().y() * delta_time,
            physic.position().z() + physic.position_dt().z() * delta_time,
            physic.radius());
    }

    glm::vec4 WorldSimulator::GetColor(const proto::Element& element) const {
        return glm::vec4(
            element.color().x(),
            element.color().y(),
            element.color().z(),
            1.0);
    }

    glm::vec4 WorldSimulator::GetColor(
        const proto::Character& character) const
    {
        return glm::vec4(
            character.color().x(),
            character.color().y(),
            character.color().z(),
            2.0);
    }

    proto::Character WorldSimulator::GetCharacterByName(
        const std::string& name) const
    {
        std::lock_guard l(mutex_);
        for (auto character : characters_) {
            if (character.name() == name) {
                return character;
            }
        }
        return proto::Character{};
    }

    void WorldSimulator::SetCharacter(const proto::Character& character) {
        std::lock_guard l(mutex_);
        for (auto& character_ : characters_) {
            if (character_.name() == character.name()) {
                character_.CopyFrom(character);
                return;
            }
        }
    }

    void WorldSimulator::RemoveCharacter(const std::string& name) {
        std::lock_guard l(mutex_);
        for (auto it = characters_.begin(); it != characters_.end(); ++it) {
            if (it->name() == name) {
                characters_.erase(it);
                return;
            }
        }
    }

    std::string WorldSimulator::GetPotentialHit(
        const proto::Character& me) const
    {
        std::lock_guard l(mutex_);
        for (const auto& element : elements_) {
            if (IsIntersecting(me.physic(), element.physic())) {
                return element.name();
            }
        }
        for (const auto& character : characters_) {
            if (me.name() == character.name()) continue;
            if (me.physic().mass() < character.physic().mass()) continue;
            if (IsIntersecting(me.physic(), character.physic())) {
                return character.name();
            }
        }
        return "";
    }

    proto::Physic WorldSimulator::GetPlanet() const {
        std::lock_guard l(mutex_);
        for (const auto& element : elements_) {
            if (element.type_enum() == proto::TYPE_GROUND) {
                return element.physic();
            }
        }
        return proto::Physic{};
    }

} // End namespace darwin.
