#include "Server/convert_math.h"

namespace darwin {

    glm::vec3 ConvertProtoVector3(const proto::Vector3& vector3) {
        glm::vec3 result;
        result.x = vector3.x();
        result.y = vector3.y();
        result.z = vector3.z();
        return result;
    }

    glm::quat ConvertProtoVector4(const proto::Vector4& vector4) {
        glm::quat result;
        result.x = vector4.x();
        result.y = vector4.y();
        result.z = vector4.z();
        result.w = vector4.w();
        return result;
    }
    
} // End namespace darwin.
