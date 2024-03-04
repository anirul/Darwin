#pragma once

#include <glm/glm.hpp>

#include "Common/darwin_service.pb.h"
#include "Common/darwin_constant.h"

namespace darwin {

    glm::dvec3 ApplyPhysic(
        const proto::Physic& physic_source, 
        const proto::Physic& physic_target);

    double UpdateObject(
        proto::Physic& physic, 
        glm::dvec3 force, 
        double delta_time);

    // Function to cancel the vertical component of the velocity vector.
    glm::dvec3 CancelVerticalComponent(
        glm::dvec3 velocity, 
        glm::dvec3 character_up);

    proto::StatusEnum CorrectSurface(
        proto::Physic& physic, 
        const proto::Element& element);

} // namespace darwin.
