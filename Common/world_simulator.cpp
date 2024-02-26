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
        last_time_ = std::chrono::system_clock::now();
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

    void WorldSimulator::ApplyGForceToCharacter(
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
                force.set_x(
                    force.x() +
                    result.force_direction.x() *
                    result.force_magnitude);
                force.set_y(
                    force.y() +
                    result.force_direction.y() *
                    result.force_magnitude);
                force.set_z(
                    force.z() +
                    result.force_direction.z() *
                    result.force_magnitude);
            }
            // Update the g part of the character.
            *character.mutable_g_normal() = Minus(Normalize(force));
            character.set_g_force(GetLength(force));
            // Update the physic part of the character.
            UpdateObject(
                *character.mutable_physic(),
                force,
                delta_time);
            // Correct the surface.
            for (auto& element : static_elements) {
                CorrectSurface(
                    *character.mutable_physic(),
                    element);
            }
        }
    }

    void WorldSimulator::UpdateTime() {
        if (!started_) return;
        auto now = std::chrono::system_clock::now();
        auto elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now - last_time_);
        time_ += elapsed.count() / 1000.0;
        last_time_ = now;
        // Get gravity forces.
        std::vector<proto::Element> static_elements = GetGForceElements();
        // Apply gravity forces to characters.
        ApplyGForceToCharacter(static_elements, elapsed.count() / 1000.0);
    }

    UniformEnum WorldSimulator::GetUniforms() {
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
        const std::string& name) const
    {
        for (auto character: characters_) {
            if (character.name() == name) {
                return character;
            }
        }
        return proto::Character{};
    }

} // End namespace darwin.
