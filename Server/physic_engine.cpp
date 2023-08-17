#include "Server/physic_engine.h"

#include "Server/convert_math.h"

namespace darwin {

glm::vec3 PhysicEngine::ComputeGravitationalForce(
    const proto::Physic& a, const proto::Physic& b) const {
  glm::vec3 r = ProtoVector2Glm(b.position()) - ProtoVector2Glm(b.position());
  float distanceSquared = glm::length(r) * glm::length(r);
  float F = (G * a.mass() * b.mass()) / distanceSquared;
  return glm::normalize(r) * F;
}

void PhysicEngine::ComputeElementInfo(double delta) {
  std::vector<proto::Element> element_grounds;
  for (auto& p : element_infos_) {
    auto& element_info = p.second;
    if (element_info.element.type_enum() == proto::Element::GROUND) {
      element_grounds.push_back(element_info.element);
    }
  }
  std::vector<glm::vec3> forces(element_grounds.size(), glm::vec3(0.0f));

  for (size_t i = 0; i < element_grounds.size(); ++i) {
    for (size_t j = i + 1; j < element_grounds.size(); ++j) {
      glm::vec3 F = ComputeGravitationalForce(element_grounds[i].physic(),
                                              element_grounds[j].physic());
      forces[i] += F;
      forces[j] -= F;
    }
  }

  for (size_t i = 0; i < element_grounds.size(); ++i) {
    auto velocity = ProtoVector2Glm(element_grounds[i].physic().velocity()) +
                    (forces[i] / element_grounds[i].physic().mass()) *
                        static_cast<float>(delta);
    *element_grounds[i].mutable_physic()->mutable_velocity() =
        Glm2ProtoVector(velocity);
    auto position = ProtoVector2Glm(element_grounds[i].physic().position()) +
                    ProtoVector2Glm(element_grounds[i].physic().velocity()) *
                        static_cast<float>(delta);
    *element_grounds[i].mutable_physic()->mutable_position() =
        Glm2ProtoVector(position);
  }
}

void PhysicEngine::ComputePlayerInfo(double delta) {}

}  // namespace darwin