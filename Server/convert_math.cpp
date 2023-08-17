#include "Server/convert_math.h"

namespace darwin {

glm::vec3 ProtoVector2Glm(const proto::Vector3& vector3) {
  glm::vec3 result;
  result.x = vector3.x();
  result.y = vector3.y();
  result.z = vector3.z();
  return result;
}

glm::quat ProtoVector2Glm(const proto::Vector4& vector4) {
  glm::quat result;
  result.x = vector4.x();
  result.y = vector4.y();
  result.z = vector4.z();
  result.w = vector4.w();
  return result;
}

proto::Vector3 Glm2ProtoVector(const glm::vec3& vector3) {
  proto::Vector3 result;
  result.set_x(vector3.x);
  result.set_y(vector3.y);
  result.set_z(vector3.z);
  return result;
}

proto::Vector4 Glm2ProtoVector(const glm::quat& vector4) {
  proto::Vector4 result;
  result.set_w(vector4.w);
  result.set_x(vector4.x);
  result.set_y(vector4.y);
  result.set_z(vector4.z);
  return result;
}

}  // End namespace darwin.
