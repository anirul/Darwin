#include "stl_proto_wrapper.h"

namespace darwin {

    bool operator==(const proto::Player& player_left, const proto::Player& player_right) {
        return google::protobuf::util::MessageDifferencer::Equals(player_left, player_right);
    }

    bool operator==(const proto::Element& element_left, const proto::Element& element_right) {
        return google::protobuf::util::MessageDifferencer::Equals(element_left, element_right);
    }

    proto::Element CreateBasicElement(
        const std::string& name,
        proto::Element::TypeEnum type_enum,
        proto::Vector3 vector3,
        double mass,
        double radius)
    {
        proto::Element element{};
        element.set_name(name);
        element.set_type_enum(type_enum);
        proto::Physic physic{};
        *physic.mutable_position() = vector3;
        physic.set_mass(mass);
        physic.set_radius(radius);
        *element.mutable_physic() = physic;
        return element;
    }

    proto::Player CreateBasicPlayer(
        const std::string& name,
        proto::Vector3 vector3,
        double mass,
        double radius)
    {
        proto::Player player{};
        player.set_name(name);
        proto::Physic physic{};
        *physic.mutable_position() = vector3;
        physic.set_mass(mass);
        physic.set_radius(radius);
        *player.mutable_physic() = physic;
        return player;
    }

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

} // namespace darwin.