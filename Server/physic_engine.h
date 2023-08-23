#pragma once

#include <glm/glm.hpp>

#include "Common/darwin_service.grpc.pb.h"
#include "Server/element_info.h"
#include "Server/player_info.h"

namespace darwin {

    // G global constant.
    constexpr double G = 6.67430e-11;

    class PhysicEngine {
    public:
        PhysicEngine(
            std::map<std::string, ElementInfo>& element_infos,
            std::map<std::string, PlayerInfo>& player_infos)
            : element_infos_(element_infos), player_infos_(player_infos) {}
        void ComputeAllInfo(double now);

    private:
        void ComputeElementInfo(
            double now,
            const std::vector<proto::Physic>& ground_physics);
        void ComputePlayerInfo(
            double now,
            const std::vector<proto::Physic>& ground_physics);
        glm::vec3 ComputeGravitationalForce(
            const proto::Physic& a,
            const proto::Physic& b) const;
        void ComputeGravitationBetweenGround(
            const std::vector<double>& times, 
            double now,
            std::vector<proto::Physic>& physics) const;
        void ComputeGravitation(
            const std::vector<double>& times, double now,
            std::vector<proto::Physic>& physics,
            const std::vector<proto::Physic>& ground_physics) const;
        std::vector<proto::Physic> GetElementPhysics(
            proto::Element::TypeEnum type_enum) const;
        std::vector<double> GetElementTimes(
            proto::Element::TypeEnum type_enum) const;
        void SetElementPhysics(
            proto::Element::TypeEnum type_enum,
            const std::vector<proto::Physic>& physics);
        bool IsIntersect(const proto::Physic& a, const proto::Physic& b) const;
        void ReactIntersectGtoundDynamic(
            const proto::Physic& a, 
            proto::Physic& b);

    protected:
        std::map<std::string, ElementInfo>& element_infos_;
        std::map<std::string, PlayerInfo>& player_infos_;
    };

}  // namespace darwin