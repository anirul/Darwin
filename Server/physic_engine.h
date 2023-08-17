#pragma once

#include <glm/glm.hpp>

#include "Common/darwin_service.grpc.pb.h"
#include "Server/element_info.h"
#include "Server/player_info.h"

namespace darwin {

constexpr double G = 6.67430e-11;

class PhysicEngine {
 public:
  PhysicEngine(std::map<std::string, ElementInfo>& element_infos,
               std::map<std::string, PlayerInfo>& player_infos)
      : element_infos_(element_infos), player_infos_(player_infos) {}
  void ComputeElementInfo(double delta);
  void ComputePlayerInfo(double delta);
  glm::vec3 ComputeGravitationalForce(const proto::Physic& a,
                                      const proto::Physic& b) const;
  void ComputeGravitationBetweenGround(
      double delta, std::vector<proto::Physic>& physics) const;
  std::vector<proto::Physic> GetElementPhysics(
      proto::Element::TypeEnum type_enum) const;
  void SetElementPhysics(proto::Element::TypeEnum type_enum,
                         const std::vector<proto::Physic>& physics);

 protected:
  std::map<std::string, ElementInfo>& element_infos_;
  std::map<std::string, PlayerInfo>& player_infos_;
};

}  // namespace darwin