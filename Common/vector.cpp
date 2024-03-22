#include "vector.h"

#include <random>
#include <array>

namespace darwin {

    proto::Vector2 CreateVector2(
        double x,
        double y)
    {
        proto::Vector2 vector2{};
        vector2.set_x(x);
        vector2.set_y(y);
        return vector2;
    }

    proto::Vector3 CreateVector3(
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

    proto::Vector4 CreateVector4(
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

    double Length(const proto::Vector3& vector3)
    {
        return std::sqrt(Dot(vector3, vector3));
    }

    double Distance(
        const proto::Vector3& vector3_left,
        const proto::Vector3& vector3_right)
    {
        return Length(vector3_left - vector3_right);
    }

    proto::Vector3 operator+(
        const proto::Vector3& vector3_left,
        const proto::Vector3& vector3_right)
    {
        proto::Vector3 vector3{};
        vector3.set_x(vector3_left.x() + vector3_right.x());
        vector3.set_y(vector3_left.y() + vector3_right.y());
        vector3.set_z(vector3_left.z() + vector3_right.z());
        return vector3;
    }

    proto::Vector3 operator-(
        const proto::Vector3& vector3_left,
        const proto::Vector3& vector3_right)
    {
        proto::Vector3 vector3{};
        vector3.set_x(vector3_left.x() - vector3_right.x());
        vector3.set_y(vector3_left.y() - vector3_right.y());
        vector3.set_z(vector3_left.z() - vector3_right.z());
        return vector3;
    }

    double Dot(
        const proto::Vector3& vector3_left,
        const proto::Vector3& vector3_right)
    {
        return 
            vector3_left.x() * vector3_right.x() +
            vector3_left.y() * vector3_right.y() +
            vector3_left.z() * vector3_right.z();
    }

    proto::Vector3 Cross(
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
        double length = Length(vector3);
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

    proto::Vector3 CreateRandomNormalizedColor(
        std::vector<proto::Vector3>::const_iterator color_begin,
        std::vector<proto::Vector3>::const_iterator color_end)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        auto distance = std::distance(color_begin, color_end);
        std::uniform_int_distribution<int> dis(0, distance - 1);
        return Normalize(*(color_begin + dis(gen)));
    }

    bool IsInColorRange(
        const proto::Vector3& color,
        std::vector<proto::Vector3>::const_iterator color_begin,
        std::vector<proto::Vector3>::const_iterator color_end)
    {
        const auto normalized_color = Normalize(color);
        for (auto it = color_begin; it != color_end; ++it) {
            if (Dot(normalized_color, *it) > 0.999f) {
                return true;
            }
        }
        return false;
    }

    proto::Vector3 operator-(const proto::Vector3& vector3)
    {
        proto::Vector3 result{};
        result.set_x(-vector3.x());
        result.set_y(-vector3.y());
        result.set_z(-vector3.z());
        return result;
    }

    proto::Vector3 operator*(
        const proto::Vector3& vector3,
        double scalar)
    {
        proto::Vector3 result{};
        result.set_x(vector3.x() * scalar);
        result.set_y(vector3.y() * scalar);
        result.set_z(vector3.z() * scalar);
        return result;
    }

    proto::Vector3 ProjectOnPlane(
        const proto::Vector3& vector3,
        const proto::Vector3& plane_normal)
    {
        double dot_VN = Dot(vector3, plane_normal);
        double normal_squared = Dot(plane_normal, plane_normal);
        proto::Vector3 proj_V_on_N = plane_normal * (dot_VN / normal_squared);
        proto::Vector3 v_plane = vector3 - proj_V_on_N;
        return Normalize(v_plane) * Length(vector3);
    }

} // End namespace darwin.
