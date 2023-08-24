#pragma once

#include "Server/world_state.h"
#include <gtest/gtest.h>

namespace test {

    class WorldStateTest : public testing::Test {
    public:
        WorldStateTest() = default;
        proto::Element CreateBasicElement(
            const std::string& name,
            proto::Element::TypeEnum type_enum,
            proto::Vector3 vector3,
            double mass,
            double radius) const;
        proto::Player CreateBasicPlayer(
            const std::string& name,
            proto::Vector3 vector3,
            double mass,
            double radius) const;
        proto::Vector3 CreateBasicVector3(double x, double y, double z) const;

    protected:
        std::unique_ptr<darwin::WorldState> world_state_;
    };

} // namespace test.
