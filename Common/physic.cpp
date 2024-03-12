#include "Common/physic.h"

#include "Common/vector.h"
#include "Common/convert_math.h"

namespace darwin {

    glm::dvec3 ApplyPhysic(
        const proto::Physic& physic_source,
        const proto::Physic& physic_target) 
    {
        glm::dvec3 distance_vector = 
            ProtoVector2Glm(physic_source.position()) - 
            ProtoVector2Glm(physic_target.position());
        double distance = glm::distance(
            ProtoVector2Glm(physic_source.position()),
            ProtoVector2Glm(physic_target.position()));
        double force_magnitude = 
            GRAVITATIONAL_CONSTANT * 
            (physic_source.mass() * physic_target.mass()) /
            (distance * distance);
        glm::dvec3 force_direction = glm::normalize(distance_vector);
        return force_direction * force_magnitude;
    }

    double UpdateObject(
        proto::Physic& physic,
        glm::dvec3 force,
        double delta_time)
    {
        // Compute the acceleration vector.
        // a = F / m
        glm::dvec3 acceleration = force / physic.mass();
        // Update the speed.
        // v(t) = v0 + at
        auto updated_speed = 
            ProtoVector2Glm(physic.position_dt()) + acceleration * delta_time;
        // Update the position.
        // x(t) = x0 + v0t + 0.5at^2
        auto updated_position = 
            ProtoVector2Glm(physic.position_dt()) * delta_time +
            acceleration * (delta_time * delta_time * 0.5);
        // Update the speed.
        physic.mutable_position_dt()->CopyFrom(Glm2ProtoVector(updated_speed));
        // Update the position.
        physic.mutable_position()->CopyFrom(
            physic.position() + Glm2ProtoVector(updated_position));
        return acceleration.length();
    }

    glm::dvec3 CancelVerticalComponent(
        glm::dvec3 velocity, 
        glm::dvec3 character_up) 
    {
        // Project velocity on character_up.
        double projectionLength = 
            glm::dot(velocity, character_up) / 
            glm::dot(character_up, character_up);
        glm::dvec3 projection = character_up * projectionLength;

        // Subtract the projection from the original velocity to remove the 
        // vertical component.
        return velocity - projection;
    }

    proto::StatusEnum CorrectSurface(
        proto::Physic& physic,
        const proto::Element& element)
    {
        if (element.type_enum() == proto::TYPE_GROUND) {
            auto distance =
                glm::distance(
                    ProtoVector2Glm(physic.position()), 
                    ProtoVector2Glm(element.physic().position()));
            if (distance < (physic.radius() + element.physic().radius())) {
                auto normal = 
                    glm::normalize(
                        ProtoVector2Glm(physic.position()) - 
                        ProtoVector2Glm(element.physic().position()));
                auto new_position = 
                    normal * (physic.radius() + element.physic().radius());
                physic.mutable_position()->CopyFrom(
                    Glm2ProtoVector(new_position));
                // Cancel the vertical component of the velocity.
                physic.mutable_position_dt()->CopyFrom(
                    Glm2ProtoVector(CancelVerticalComponent(
                        ProtoVector2Glm(physic.position_dt()), normal)));
                return proto::STATUS_ON_GROUND;
            }
            return proto::STATUS_JUMPING;
        }
        return proto::STATUS_UNKNOWN;
    }

} // namespace darwin.
