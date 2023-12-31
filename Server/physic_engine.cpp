#include "Server/physic_engine.h"

#include "Server/convert_math.h"

namespace darwin {

    glm::vec3 PhysicEngine::ComputeGravitationalForce(
        const proto::Physic& a,
        const proto::Physic& b) const
    {
        glm::dvec3 r =
            ProtoVector2Glm(b.position()) - ProtoVector2Glm(a.position());
        double distanceSquared = glm::length(r) * glm::length(r);
        double F = (G * a.mass() * b.mass()) / distanceSquared;
        return glm::normalize(r) * F;
    }

    void PhysicEngine::ComputeGravitation(
        const std::vector<double>& times,
        double now,
        std::vector<proto::Physic>& physics,
        const std::vector<proto::Physic>& ground_physics) const
    {
        for (auto i = 0; i < physics.size(); ++i) {
            glm::dvec3 F(0.0f);
            for (const auto& ground_physic : ground_physics) {
                F += ComputeGravitationalForce(physics[i], ground_physic);
            }
            double delta = now - times[i];
            glm::dvec3 velocity = ProtoVector2Glm(physics[i].velocity()) +
                (F / physics[i].mass()) * delta;
            *physics[i].mutable_velocity() = Glm2ProtoVector(velocity);
            auto position =
                ProtoVector2Glm(physics[i].position()) +
                ProtoVector2Glm(physics[i].velocity()) * delta;
            *physics[i].mutable_position() = Glm2ProtoVector(position);
        }
    }

    void PhysicEngine::ComputeGravitationBetweenGround(
        const std::vector<double>& times,
        double now,
        std::vector<proto::Physic>& physics) const
    {
        if (physics.empty()) {
            std::cerr << "No bodies?" << std::endl;
            return;
        }
        // Create a list of forces to be applied to these elements.
        std::vector<glm::dvec3> forces(physics.size(), glm::dvec3(0.0));
        // Fill it up.
        for (size_t i = 0; i < physics.size(); ++i) {
            for (size_t j = i + 1; j < physics.size(); ++j) {
                glm::dvec3 F =
                    ComputeGravitationalForce(physics[i], physics[j]);
                forces[i] += F;
                forces[j] -= F;
            }
        }
        // Now compute the new position according to this simulation.
        for (size_t i = 0; i < physics.size(); ++i) {
            double delta = now - times[i];
            auto velocity = ProtoVector2Glm(physics[i].velocity()) +
                (forces[i] / physics[i].mass()) * delta;
            *physics[i].mutable_velocity() = Glm2ProtoVector(velocity);
            auto position =
                ProtoVector2Glm(physics[i].position()) +
                ProtoVector2Glm(physics[i].velocity()) * delta;
            *physics[i].mutable_position() = Glm2ProtoVector(position);
        }
    }

    std::vector<proto::Physic> PhysicEngine::GetElementPhysics(
        proto::Element::TypeEnum type_enum) const
    {
        std::vector<proto::Physic> physics;
        for (const auto& [_, element_info] : element_infos_) {
            if (element_info.element.type_enum() == type_enum) {
                physics.push_back(element_info.element.physic());
            }
        }
        return physics;
    }

    std::vector<double> PhysicEngine::GetElementTimes(
        proto::Element::TypeEnum type_enum) const
    {
        std::vector<double> times;
        for (const auto& [_, element_info] : element_infos_) {
            if (element_info.element.type_enum() == type_enum) {
                times.push_back(element_info.time);
            }
        }
        return times;
    }

    void PhysicEngine::SetElementPhysics(
        proto::Element::TypeEnum type_enum,
        const std::vector<proto::Physic>& physics)
    {
        std::size_t i = 0;
        for (auto& p : element_infos_) {
            if (i > physics.size()) {
                std::cerr << "Size mismatch." << std::endl;
                return;
            }
            if (p.second.element.type_enum() == type_enum) {
                *p.second.element.mutable_physic() = physics[i];
                ++i;
            }
        }
    }

    void PhysicEngine::SetElementTimes(
        proto::Element::TypeEnum type_enum, 
        double time)
    {
        for (auto& p : element_infos_) {
            if (p.second.element.type_enum() == type_enum) {
                p.second.time = time;
            }
        }
    }

    bool PhysicEngine::IsIntersect(
        const proto::Physic& a,
        const proto::Physic& b) const
    {
        glm::dvec3 diff =
            ProtoVector2Glm(a.position()) - ProtoVector2Glm(b.position());
        double distanceSquared = glm::dot(diff, diff);
        double sumOfRadii = a.radius() + b.radius();
        return distanceSquared <= sumOfRadii * sumOfRadii;
    }

    void PhysicEngine::ReactIntersectGtoundDynamic(
        const proto::Physic& a,
        proto::Physic& b)
    {
        glm::dvec3 collisionNormal =
            glm::normalize(
                ProtoVector2Glm(a.position()) - ProtoVector2Glm(b.position()));
        *b.mutable_velocity() = a.velocity();
        *b.mutable_position() =
            Glm2ProtoVector(
                -collisionNormal * (a.radius() + b.radius()) + 
                ProtoVector2Glm(a.position()));
    }

    void PhysicEngine::ComputeAllInfo(double now) {
        // Create a list of element that can interact between each other, 
        // things like planets and ground elements.
        std::vector<proto::Physic> ground_physics =
            GetElementPhysics(proto::Element::GROUND);
        std::vector<double> times = GetElementTimes(proto::Element::GROUND);
        // Compute the gravitational force between them.
        ComputeGravitationBetweenGround(times, now, ground_physics);
        // Update the physics.
        SetElementPhysics(proto::Element::GROUND, ground_physics);
        ComputeElementInfo(now, ground_physics);
        ComputePlayerInfo(now, ground_physics);
    }

    void PhysicEngine::ComputeElementInfo(
        double now,
        const std::vector<proto::Physic>& ground_physics)
    {
        // All expect ground elements.
        std::vector<proto::Element::TypeEnum> types = {
            proto::Element::EXPLOSION,
            proto::Element::UPGRADE,
            proto::Element::GREEN,
            proto::Element::BROWN,
            proto::Element::WATER
        };
        for (const auto type : types) {
            std::vector<proto::Physic> physics = GetElementPhysics(type);
            std::vector<double> times = GetElementTimes(type);
            // Check if there is any element to treat.
            if (physics.empty() && times.empty()) {
                continue;
            }
            // Check if the size are the same.
            if (physics.size() != times.size()) {
                std::cerr << "Size mismatch." << std::endl;
                continue;
            }
            // Compute the gravitational force between them.
            ComputeGravitation(times, now, physics, ground_physics);
            for (auto i = 0; i < ground_physics.size(); ++i) {
                for (auto j = 0; j < physics.size(); ++j) {
                    // Check if there is any intersection.
                    if (IsIntersect(ground_physics[i], physics[j])) {
                        // React to the intersection.
                        ReactIntersectGtoundDynamic(
                            ground_physics[i],
                            physics[j]);
                    }
                }
            }
            SetElementPhysics(type, physics);
            SetElementTimes(type, now);
        }
    }

    void PhysicEngine::ComputePlayerInfo(
        double now,
        const std::vector<proto::Physic>& ground_physics)
    {
        std::vector<proto::Physic> player_physics;
        for (auto& player_info : player_infos_) {
            double time = player_info.second.time;
            auto physic = player_info.second.player.physic();
            glm::dvec3 F(0.0);
            for (const auto& ground_physic : ground_physics) {
                if (IsIntersect(ground_physic, physic)) {
                    ReactIntersectGtoundDynamic(ground_physic, physic);
                } else {
                    F += ComputeGravitationalForce(physic, ground_physic);
                }
            }
            double delta = now - time;
            auto velocity = ProtoVector2Glm(physic.velocity()) +
                (F / physic.mass()) * delta;
            *physic.mutable_velocity() = Glm2ProtoVector(velocity);
            auto position =
                ProtoVector2Glm(physic.position()) +
                ProtoVector2Glm(physic.velocity()) * delta;
            *physic.mutable_position() = Glm2ProtoVector(position);
            *player_info.second.player.mutable_physic() = physic;
            // Update the time.
            player_info.second.time = now;
        }
    }

}  // namespace darwin.
