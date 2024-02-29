#include "physic.h"
#include "vector.h"

namespace darwin {

    GResult ApplyPhysic(
        const proto::Physic& physic_source,
        const proto::Physic& physic_target) 
    {
        proto::Vector3 distance_vector = 
            Add(
                physic_source.position(),
                Minus(physic_target.position()));
        double distance = Distance(
            physic_source.position(),
            physic_target.position());
        double force_magnitude = 
            GRAVITATIONAL_CONSTANT * 
            (physic_source.mass() * physic_target.mass()) /
            (distance * distance);
        proto::Vector3 force_direction = Normalize(distance_vector);
        return GResult{ force_direction, force_magnitude };
    }

    float UpdateObject(
        proto::Physic& physic,
        const proto::Vector3& force,
        double delta_time)
    {
        // Compute the acceleration vector.
        // a = F / m
        proto::Vector3 acceleration = 
            MultiplyVector3ByScalar(force, 1.0 / physic.mass());
        // Update the speed.
        // v(t) = v0 + at
        auto updated_speed = 
            Add(
                physic.position_dt(),
                MultiplyVector3ByScalar(acceleration, delta_time));
        // Update the position.
        // x(t) = x0 + v0t + 0.5at^2
        auto updated_position =
            Add(
                MultiplyVector3ByScalar(physic.position_dt(), delta_time),
                MultiplyVector3ByScalar(
                    acceleration, 
                    delta_time * delta_time * 0.5));
        // Update the speed.
        physic.mutable_position_dt()->CopyFrom(updated_speed);
        // Update the position.
        physic.mutable_position()->CopyFrom(
            Add(physic.position(), updated_position));
        return GetLength(acceleration);
    }

    proto::Vector3 CancelVerticalComponent(
        const proto::Vector3& velocity, 
        const proto::Vector3& character_up) 
    {
        // Project velocity on character_up.
        float projectionLength = 
            DotProduct(velocity, character_up) / 
            DotProduct(character_up, character_up);
        proto::Vector3 projection = 
            MultiplyVector3ByScalar(character_up, projectionLength);

        // Subtract the projection from the original velocity to remove the 
        // vertical component.
        return Add(velocity, Minus(projection));
    }

    proto::StatusEnum CorrectSurface(
        proto::Physic& physic,
        const proto::Element& element)
    {
        if (element.type_enum() == proto::TYPE_GROUND) {
            auto distance =
                Distance(physic.position(), element.physic().position());
            if (distance < (physic.radius() + element.physic().radius())) {
                auto normal = 
                    Normalize(
                        Add(
                            physic.position(), 
                            Minus(element.physic().position())));
                auto new_position_delta =
                    MultiplyVector3ByScalar(
                        normal,
                        physic.radius() + element.physic().radius());
                physic.mutable_position()->CopyFrom(
                    Add(element.physic().position(), new_position_delta));
                // Cancel the vertical component of the velocity.
                physic.mutable_position_dt()->CopyFrom(
                    CancelVerticalComponent(
                        physic.position_dt(),
                        Normalize(
                            Add(
                                physic.position(),
                                Minus(element.physic().position())))));
                return proto::STATUS_ON_GROUND;
            }
            return proto::STATUS_JUMPING;
        }
        return proto::STATUS_UNKNOWN;
    }

} // namespace darwin.
