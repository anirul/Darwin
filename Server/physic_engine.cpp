#include "Server/physic_engine.h"
#include "Server/convert_math.h"

namespace darwin {
    
// Compute the gravitational force between two bodies
glm::vec3 PhysicEngine::ComputeGravitationalForce(
    const proto::Physic& a, const proto::Physic& b) const {
  glm::vec3 r = 
      ConvertProtoVector3(b.position()) - ConvertProtoVector3(b.position());
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
}

void PhysicEngine::ComputePlayerInfo(double delta) {}

}