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
        // Apply it to characters.
        for (auto& character : characters_) {
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

    bool WorldSimulator::IsClose(
        const proto::Vector3& normal,
        const proto::Vector3& position) const
    {
        return DotProduct(normal, position) > 0.8;
    }

    UniformEnum WorldSimulator::GetCloseUniforms(
        const proto::Vector3& normal) const
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
            if (IsClose(normal, Normalize(character.physic().position()))) {
                uniform_enum.spheres.push_back(GetSphere(character.physic()));
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
            1.0);
    }

    proto::Character WorldSimulator::GetCharacterByName(
        const std::string& name)
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
        throw std::runtime_error("No planet found.");
    }

} // End namespace darwin.
