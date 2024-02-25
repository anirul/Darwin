#include "physic.h"
#include "vector.h"

namespace darwin {

    GResult ApplyPhysic(
        const proto::Physic& physic_source,
        const proto::Physic& physic_target) 
    {
        proto::Vector3 distance_vector = CreateBasicVector3(
            physic_source.position().x() - physic_target.position().x(),
            physic_source.position().y() - physic_target.position().y(),
            physic_source.position().z() - physic_target.position().z());
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

    void UpdateObject(proto::Physic& physic,
        const proto::Vector3& force,
        double delta_time)
    {
        double acceleration = force.x() / physic.mass();
        physic.mutable_position_dt()->set_x(
            physic.position_dt().x() + acceleration * delta_time);
        physic.mutable_position()->set_x(
            physic.position().x() + 
            physic.position_dt().x() * delta_time);
        acceleration = force.y() / physic.mass();
        physic.mutable_position_dt()->set_y(
            physic.position_dt().y() + acceleration * delta_time);
        physic.mutable_position()->set_y(
            physic.position().y() + 
            physic.position_dt().y() * delta_time);
        acceleration = force.z() / physic.mass();
        physic.mutable_position_dt()->set_z(
            physic.position_dt().z() + acceleration * delta_time);
        physic.mutable_position()->set_z(
            physic.position().z() + 
            physic.position_dt().z() * delta_time);
    }

    void CorrectSurface(
        proto::Physic& physic,
        const proto::Element& element)
    {
        if (element.type_enum() == proto::TYPE_GROUND) {
            auto distance = 
                Distance(physic.position(), element.physic().position());
            if (distance < (physic.radius() + element.physic().radius())) {
                auto normal = Normalize(
                    CreateBasicVector3(
                        physic.position().x() - 
                        element.physic().position().x(),
                        physic.position().y() - 
                        element.physic().position().y(),
                        physic.position().z() - 
                        element.physic().position().z()));
                auto new_position_delta = 
                    MultiplyVector3ByScalar(
                        normal, 
                        physic.radius() + element.physic().radius());
                *physic.mutable_position() = 
                    Add(element.physic().position(), new_position_delta);
            }
        }
    }

} // namespace darwin.
