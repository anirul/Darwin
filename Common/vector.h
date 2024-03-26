#pragma once

#include "darwin_service.pb.h"

namespace darwin {

    proto::Vector2 CreateVector2(double x, double y);
    proto::Vector3 CreateVector3(double x, double y, double z);
    proto::Vector4 CreateVector4(double x, double y, double z, double w);
    double Length(const proto::Vector3& vector3);
    double Distance(
        const proto::Vector3& vector3_left, 
        const proto::Vector3& vector3_right);
    proto::Vector3 operator+(
        const proto::Vector3& vector3_left,
        const proto::Vector3& vector3_right);
    proto::Vector3 operator-(
        const proto::Vector3& vector3_left,
        const proto::Vector3& vector3_right);
    double Dot(
        const proto::Vector3& vector3_left,
        const proto::Vector3& vector3_right);
    proto::Vector3 Cross(
        const proto::Vector3& vector3_left,
        const proto::Vector3& vector3_right);
    proto::Vector3 Normalize(const proto::Vector3& vector3);
    proto::Vector3 CreateRandomNormalizedVector3();
    proto::Vector3 CreateRandomNormalizedColor(
        std::vector<proto::Vector3>::const_iterator color_begin,
        std::vector<proto::Vector3>::const_iterator color_end);
    bool IsInColorRange(
        const proto::Vector3& color, 
        std::vector<proto::Vector3>::const_iterator color_begin,
        std::vector<proto::Vector3>::const_iterator color_end);
    proto::Vector3 operator-(const proto::Vector3& vector3);
    proto::Vector3 operator*(
        const proto::Vector3& vector3, double scalar);
    proto::Vector3 ProjectOnPlane(
        const proto::Vector3& vector3, 
        const proto::Vector3& plane_normal);

} // End namespace darwin.
