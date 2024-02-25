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
        void SetName(const std::string& name);
        std::string GetName() const;
        void UpdateData(
            const std::vector<proto::Element>& elements,
            const std::vector<proto::Character>& characters,
            double time);
        void UpdateTime();
        UniformEnum GetUniforms();

    private:
        std::string name_;
        bool started_ = false;
        std::vector<proto::Element> elements_;
        std::vector<proto::Character> characters_;
        double time_;
        std::chrono::time_point<std::chrono::system_clock> last_time_;
    };

} // End namespace darwin.
