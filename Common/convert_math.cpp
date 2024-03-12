#include "Common/convert_math.h"

#include <random>
#include <cmath>
#include <format>

#include "Common/darwin_constant.h"
#include "Common/vector.h"

namespace darwin {

    glm::dvec3 ProtoVector2Glm(const proto::Vector3& vector3) {
        glm::dvec3 result;
        result.x = vector3.x();
        result.y = vector3.y();
        result.z = vector3.z();
        return result;
    }

    glm::dquat ProtoVector2Glm(const proto::Vector4& vector4) {
        glm::dquat result;
        result.x = vector4.x();
        result.y = vector4.y();
        result.z = vector4.z();
        result.w = vector4.w();
        return result;
    }

    proto::Vector3 Glm2ProtoVector(const glm::dvec3& vector3) {
        proto::Vector3 result;
        result.set_x(vector3.x);
        result.set_y(vector3.y);
        result.set_z(vector3.z);
        return result;
    }

    proto::Vector4 Glm2ProtoVector(const glm::dquat& vector4) {
        proto::Vector4 result;
        result.set_w(vector4.w);
        result.set_x(vector4.x);
        result.set_y(vector4.y);
        result.set_z(vector4.z);
        return result;
    }

    glm::vec3 RandomVec3() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(-1.0, 1.0);
        return glm::vec3(dis(gen), dis(gen), dis(gen));
    }

    double GetRadiusFromVolume(double volume) {
        return std::cbrt((3.0 * volume) / (4.0 * PI));
    }

    bool IsIntersecting(
        const proto::Physic& character,
        const proto::Physic& element)
    {
        double distance =
            Distance(character.position(), element.position());
        double radius_sum = character.radius() + element.radius();
        return distance < radius_sum;
    }

    bool IsAlmostIntersecting(
        const proto::Physic& character,
        const proto::Physic& element)
    {
        auto dot = 
            DotProduct(
                Normalize(character.position()), 
                Normalize(element.position()));
        return dot > ALMOST_INTERSECT;
    }

}  // End namespace darwin.
