#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "darwin_service.pb.h"

namespace darwin {

    struct UniformEnum {
        std::vector<glm::vec4> spheres;
        std::vector<glm::vec4> colors;
    };

    class WorldSimulator {
    public:
        void SetUserName(const std::string& name);
        std::string GetUserName() const;
        void UpdateData(
            const std::vector<proto::Element>& elements,
            const std::vector<proto::Character>& characters,
            double time);
        void UpdateTime();
        UniformEnum GetUniforms();
        proto::Character GetCharacterByName(const std::string& name);
        void SetCharacter(const proto::Character& character);

    public:
        std::vector<proto::Element> GetElements() const {
            return elements_;
        }
        std::vector<proto::Character> GetCharacters() const {
            return characters_;
        }


    protected:
        std::vector<proto::Element> GetGForceElements();
        void ApplyGForceAndSpeedToCharacter(
            const std::vector<proto::Element>& static_elements,
            double delta_time);

    private:
        std::string name_;
        bool started_ = false;
        std::mutex mutex_;
        std::vector<proto::Element> elements_;
        std::vector<proto::Character> characters_;
        double time_;
        std::chrono::time_point<std::chrono::high_resolution_clock> last_time_;
    };

} // End namespace darwin.
