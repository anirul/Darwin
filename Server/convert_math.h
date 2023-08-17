#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Common/darwin_service.grpc.pb.h"

namespace darwin {
    
glm::vec3 ProtoVector2Glm(const proto::Vector3& vector3);
glm::quat ProtoVector2Glm(const proto::Vector4& vector4);
proto::Vector3 Glm2ProtoVector(const glm::vec3& vector3);
proto::Vector4 Glm2ProtoVector(const glm::quat& vector4);

} // namespace darwin
