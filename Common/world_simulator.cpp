#include "world_simulator.h"

#include <format>

#include "vector.h"
#include "physic.h"

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

    std::vector<proto::Element> WorldSimulator::GetGForceElements() {
        std::vector<proto::Element> result;
        for (auto& element : elements_) {
            if (element.type_enum() == proto::TYPE_GROUND) {
                result.push_back(element);
            }
        }
        return result;
    }

    void WorldSimulator::ApplyGForceAndSpeedToCharacter(
        const std::vector<proto::Element>& static_elements,
        double delta_time)
    {
        // Apply it to characters.
        for (auto& character : characters_) {
            proto::Vector3 force{};
            // Add all gravity forces.
            for (auto& element : static_elements) {
                auto result = ApplyPhysic(
                    element.physic(),
                    character.physic());
                force = 
                    Add(
                        force,
                        MultiplyVector3ByScalar(
                            result.force_direction,
                            result.force_magnitude));
            }
            // Update the g part of the character.
            character.mutable_g_normal()->CopyFrom(Minus(Normalize(force)));
            character.set_g_force(GetLength(force));
            // Update the physic part of the character.
            float acceleration_length = UpdateObject(
                *character.mutable_physic(),
                force,
                delta_time);
            // Correct the acceleration.
            character.set_g_force(acceleration_length);
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
        std::vector<proto::Element> static_elements = GetGForceElements();
        // Apply gravity forces to characters.
        ApplyGForceAndSpeedToCharacter(static_elements, elapsed_seconds);
    }

    UniformEnum WorldSimulator::GetUniforms() {
        std::lock_guard l(mutex_);
        UniformEnum uniform_enum;
        for (auto& element : elements_) {
            uniform_enum.spheres.push_back(
                glm::vec4(
                    element.physic().position().x(),
                    element.physic().position().y(),
                    element.physic().position().z(),
                    element.physic().radius()));
            switch (element.type_enum()) {
            case proto::TYPE_GROUND:
                uniform_enum.colors.push_back(
                    glm::vec4(glm::normalize(glm::vec3(1.0, 1.0, 1.0)), 1.0));
                break;
            case proto::TYPE_EXPLOSION:
                uniform_enum.colors.push_back(
                    glm::vec4(glm::normalize(glm::vec3(1.0, 0.0, 0.0)), 1.0));
                break;
            case proto::TYPE_GREEN:
                uniform_enum.colors.push_back(
                    glm::vec4(glm::normalize(glm::vec3(0.0, 1.0, 0.0)), 1.0));
                break;
            case proto::TYPE_WATER:
                uniform_enum.colors.push_back(
                    glm::vec4(glm::normalize(glm::vec3(0.0, 0.0, 1.0)), 1.0));
                break;
            case proto::TYPE_UPGRADE:
                uniform_enum.colors.push_back(glm::vec4(1.0, 1.0, 1.0, 1.0));
                break;
            default:
                uniform_enum.colors.push_back(glm::vec4(0.0, 0.0, 0.0, 1.0));
                break;
            }
        }
        for (auto& character : characters_) {
            uniform_enum.spheres.push_back(
                glm::vec4(
                    character.physic().position().x(),
                    character.physic().position().y(),
                    character.physic().position().z(),
                    character.physic().radius()));
            uniform_enum.colors.push_back(
                glm::vec4(
                    character.color().x(),
                    character.color().y(),
                    character.color().z(),
                    1.0));
        }
        return uniform_enum;
    }

    proto::Character WorldSimulator::GetCharacterByName(
        const std::string& name)
    {
        std::lock_guard l(mutex_);
        for (auto character: characters_) {
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

} // End namespace darwin.
