#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "darwin_service.pb.h"

namespace darwin {

    glm::dvec3 ProtoVector2Glm(const proto::Vector3& vector3);
    glm::dquat ProtoVector2Glm(const proto::Vector4& vector4);
    proto::Vector3 Glm2ProtoVector(const glm::dvec3& vector3);
    proto::Vector4 Glm2ProtoVector(const glm::dquat& vector4);
    glm::vec3 RandomVec3();
    double GetRadiusFromVolume(double volume);
    // Check for exact intersections.
    bool IsIntersecting(
        const proto::Physic& character,
        const proto::Physic& element);
    // Check for aproximative intersections.
    bool IsAlmostIntersecting(
        const proto::Physic& character,
        const proto::Physic& element);
    double GetTimeSecondNow();

} // namespace darwin
