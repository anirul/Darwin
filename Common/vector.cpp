#include "vector.h"

#include <random>

namespace darwin {

    proto::Vector3 CreateBasicVector3(
        double x,
        double y,
        double z)
    {
        proto::Vector3 vector3{};
        vector3.set_x(x);
        vector3.set_y(y);
        vector3.set_z(z);
        return vector3;
    }

    proto::Vector4 CreateBasicVector4(
        double x,
        double y,
        double z,
        double w)
    {
        proto::Vector4 vector4{};
        vector4.set_x(x);
        vector4.set_y(y);
        vector4.set_z(z);
        vector4.set_w(w);
        return vector4;
    }

    double GetLength(const proto::Vector3& vector3)
    {
        return std::sqrt(
            vector3.x() * vector3.x() +
            vector3.y() * vector3.y() +
            vector3.z() * vector3.z());
    }

    double Distance(
        const proto::Vector3& vector3_left,
        const proto::Vector3& vector3_right)
    {
        return GetLength(CreateBasicVector3(
            vector3_left.x() - vector3_right.x(),
            vector3_left.y() - vector3_right.y(),
            vector3_left.z() - vector3_right.z()));
    }

    double DotProduct(
        const proto::Vector3& vector3_left,
        const proto::Vector3& vector3_right)
    {
        return vector3_left.x() * vector3_right.x() +
            vector3_left.y() * vector3_right.y() +
            vector3_left.z() * vector3_right.z();
    }

    proto::Vector3 CrossProduct(
        const proto::Vector3& vector3_left,
        const proto::Vector3& vector3_right)
    {
        proto::Vector3 vector3{};
        vector3.set_x(
            vector3_left.y() * vector3_right.z() -
            vector3_left.z() * vector3_right.y());
        vector3.set_y(
            vector3_left.z() * vector3_right.x() -
            vector3_left.x() * vector3_right.z());
        vector3.set_z(
            vector3_left.x() * vector3_right.y() -
            vector3_left.y() * vector3_right.x());
        return vector3;
    }

    proto::Vector3 Normalize(const proto::Vector3& vector3)
    {
        double length = GetLength(vector3);
        proto::Vector3 normalized_vector3{};
        normalized_vector3.set_x(vector3.x() / length);
        normalized_vector3.set_y(vector3.y() / length);
        normalized_vector3.set_z(vector3.z() / length);
        return normalized_vector3;
    }

    proto::Vector3 CreateRandomNormalizedVector3()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(-1.0, 1.0);
        proto::Vector3 vector3{};
        vector3.set_x(dis(gen));
        vector3.set_y(dis(gen));
        vector3.set_z(dis(gen));
        return Normalize(vector3);
    }

    proto::Vector3 Minus(const proto::Vector3& vector3)
    {
        proto::Vector3 result{};
        result.set_x(-vector3.x());
        result.set_y(-vector3.y());
        result.set_z(-vector3.z());
        return result;
    }

    proto::Vector3 MultiplyVector3ByScalar(
        const proto::Vector3& vector3,
        double scalar)
    {
        proto::Vector3 result{};
        result.set_x(vector3.x() * scalar);
        result.set_y(vector3.y() * scalar);
        result.set_z(vector3.z() * scalar);
        return result;
    }


} // End namespace darwin.
