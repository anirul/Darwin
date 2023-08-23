#include "Test/Server/world_state_test.h"

namespace test {

    TEST_F(WorldStateTest, WorldStateTestBasic) {
        EXPECT_FALSE(world_state_);
        world_state_ = std::make_unique<darwin::WorldState>();
        EXPECT_TRUE(world_state_);
    }

    TEST_F(WorldStateTest, WorldStateTestAddElement) {
        EXPECT_FALSE(world_state_);
        world_state_ = std::make_unique<darwin::WorldState>();
        EXPECT_TRUE(world_state_);
        world_state_->AddElement(
            0.0, 
            CreateBasicGroundElement(
                "ground",
                CreateBasicVector3(1.0, 1.0, 1.0),
                1.0,
                1.0));
        world_state_->Update(0.1);
        auto elements = world_state_->GetElements();
        EXPECT_EQ(elements.size(), 1);
    }

    TEST_F(WorldStateTest, WorldStateTestUpdateOneElement) {
        EXPECT_FALSE(world_state_);
        world_state_ = std::make_unique<darwin::WorldState>();
        EXPECT_TRUE(world_state_);
        world_state_->AddElement(
            0.0,
            CreateBasicGroundElement(
                "ground",
                CreateBasicVector3(1.0, 1.0, 1.0),
                1.0,
                1.0));
        world_state_->Update(10.0);
        auto elements = world_state_->GetElements();
        EXPECT_EQ(elements.size(), 1);
        auto position = elements[0].physic().position();
        EXPECT_EQ(position.x(), 1.0);
        EXPECT_EQ(position.y(), 1.0);
        EXPECT_EQ(position.z(), 1.0);
    }

    TEST_F(WorldStateTest, WorldStateTestUpdateTwoElements) {
        EXPECT_FALSE(world_state_);
        world_state_ = std::make_unique<darwin::WorldState>();
        EXPECT_TRUE(world_state_);
        world_state_->AddElement(
            0.0,
            CreateBasicGroundElement(
                "ground1",
                CreateBasicVector3(-2.0, 0.0, 0.0),
                1'000'000.0,
                1.0));
        world_state_->AddElement(
            0.0,
            CreateBasicGroundElement(
                "ground2",
                CreateBasicVector3(2.0, 0.0, 0.0),
                1'000'000.0,
                1.0));
        world_state_->Update(10.0);
        auto elements = world_state_->GetElements();
        EXPECT_EQ(elements.size(), 2);
        auto position = elements[0].physic().position();
        EXPECT_LT(position.x(), 2.0);
        EXPECT_EQ(position.y(), 0.0);
        EXPECT_EQ(position.z(), 0.0);
    }

    proto::Element WorldStateTest::CreateBasicGroundElement(
        const std::string& name,
        proto::Vector3 vector3,
        double mass,
        double radius) const
    {
        proto::Element element{};
        element.set_name(name);
        proto::Physic physic{};
        *physic.mutable_position() = vector3;
        physic.set_mass(mass);
        physic.set_radius(radius);
        *element.mutable_physic() = physic;
        element.set_type_enum(proto::Element::GROUND);
        return element;
    }

    proto::Vector3 WorldStateTest::CreateBasicVector3(
        double x,
        double y,
        double z) const
    {
        proto::Vector3 vector3{};
        vector3.set_x(x);
        vector3.set_y(y);
        vector3.set_z(z);
        return vector3;
    }

}  // namespace test.