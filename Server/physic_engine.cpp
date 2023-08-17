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

void PhysicEngine::ComputeGravitationBetweenGround(
    double delta, std::vector<proto::Physic>& physics) const {
  if (physics.empty()) {
    std::cerr << "No bodies?" << std::endl;
    return;
  }
  // Create a list of forces to be applied to these elements.
  std::vector<glm::vec3> forces(physics.size(), glm::vec3(0.0f));
  // Fill it up.
  for (size_t i = 0; i < physics.size(); ++i) {
    for (size_t j = i + 1; j < physics.size(); ++j) {
      glm::vec3 F = ComputeGravitationalForce(physics[i], physics[j]);
      forces[i] += F;
      forces[j] -= F;
    }
  }
  // Now compute the new position according to this simulation.
  for (size_t i = 0; i < physics.size(); ++i) {
    auto velocity = ProtoVector2Glm(physics[i].velocity()) +
                    (forces[i] / physics[i].mass()) * static_cast<float>(delta);
    *physics[i].mutable_velocity() = Glm2ProtoVector(velocity);
    auto position =
        ProtoVector2Glm(physics[i].position()) +
        ProtoVector2Glm(physics[i].velocity()) * static_cast<float>(delta);
    *physics[i].mutable_position() = Glm2ProtoVector(position);
  }
}

std::vector<proto::Physic> PhysicEngine::GetElementPhysics(proto::Element::TypeEnum type_enum) const {
    std::vector<proto::Physic> physics;
  for (const auto& p : element_infos_) {
    const auto& element_info = p.second;
    if (element_info.element.type_enum() == proto::Element::GROUND) {
      physics.push_back(element_info.element.physic());
    }
  }
  return physics;
}

void PhysicEngine::SetElementPhysics(
    proto::Element::TypeEnum type_enum,
    const std::vector<proto::Physic>& physics) {
    std::size_t i = 0;
    for (auto& p : element_infos_) {
        if (i < physics.size()) {
            std::cerr << "Size mismatch." << std::endl;
            return;
        }
        if (p.second.element.type_enum() == type_enum) {
            *p.second.element.mutable_physic() = physics[i];
            ++i;
        }
    }
}

void PhysicEngine::ComputeElementInfo(double delta) {
  // Create a list of element that can interact between each other, things like
  // planets and ground elements.
  std::vector<proto::Physic> physics = GetElementPhysics(proto::Element::GROUND);
  ComputeGravitationBetweenGround(delta, physics);
  SetElementPhysics(proto::Element::GROUND, physics);
}

void PhysicEngine::ComputePlayerInfo(double delta) {}

}  // namespace darwin