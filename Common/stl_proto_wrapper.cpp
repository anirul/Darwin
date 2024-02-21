#include "stl_proto_wrapper.h"

#include <random>

namespace darwin {

    bool operator==(
        const proto::Character& character_left, 
        const proto::Character& character_right) {
        return google::protobuf::util::MessageDifferencer::Equals(
            character_left, 
            character_right);
    }

    bool operator==(
        const proto::Element& element_left, 
        const proto::Element& element_right) {
        return google::protobuf::util::MessageDifferencer::Equals(
            element_left, 
            element_right);
    }

    proto::Element CreateBasicElement(
        const std::string& name,
        proto::TypeEnum type_enum,
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

    proto::Character CreateBasicCharacter(
        const std::string& name,
        proto::Vector3 vector3,
        double mass,
        double radius)
    {
        proto::Character character{};
        character.set_name(name);
        proto::Physic physic{};
        *physic.mutable_position() = vector3;
        physic.set_mass(mass);
        physic.set_radius(radius);
        *character.mutable_physic() = physic;
        return character;
    }

} // namespace darwin.