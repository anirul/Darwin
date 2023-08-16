#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Common/darwin_service.grpc.pb.h"

namespace darwin {
    
glm::vec3 ConvertProtoVector3(const proto::Vector3& vector3);
glm::quat ConvertProtoVector4(const proto::Vector4& vector4);

}