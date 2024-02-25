#pragma once

#include "darwin_service.pb.h"

namespace darwin {

    constexpr double GRAVITATIONAL_CONSTANT = 6.67430e-11;

    struct GResult {
        proto::Vector3 force_direction;
        double force_magnitude;
    };

    GResult ApplyPhysic(
        const proto::Physic& physic_source, 
        const proto::Physic& physic_target);

    void UpdateObject(
        proto::Physic& physic, 
        const proto::Vector3& force, 
        double delta_time);

    void CorrectSurface(
        proto::Physic& physic, 
        const proto::Element& element);

} // namespace darwin.
