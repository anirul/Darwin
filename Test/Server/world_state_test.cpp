#include "Test/Server/world_state_test.h"

#include "Common/convert_math.h"
#include "Common/stl_proto_wrapper.h"
#include "Common/vector.h"

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
            darwin::CreateBasicElement(
                "ground",
                proto::TYPE_GROUND,
                darwin::CreateVector3(1.0, 1.0, 1.0),
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
            darwin::CreateBasicElement(
                "ground",
                proto::TYPE_GROUND,
                darwin::CreateVector3(1.0, 1.0, 1.0),
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
            darwin::CreateBasicElement(
                "ground1",
                proto::TYPE_GROUND,
                darwin::CreateVector3(-2.0, 0.0, 0.0),
                1'000'000.0,
                1.0));
        world_state_->AddElement(
            0.0,
            darwin::CreateBasicElement(
                "ground2",
                proto::TYPE_GROUND,
                darwin::CreateVector3(2.0, 0.0, 0.0),
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

    TEST_F(WorldStateTest, WorldStateTestGroundAndElement) {
        EXPECT_FALSE(world_state_);
        world_state_ = std::make_unique<darwin::WorldState>();
        EXPECT_TRUE(world_state_);
        // A planet with the mass of 1'000'000'000.0g (1'000'000kg) and a
        // radius of 10.0.
        world_state_->AddElement(
            0.0,
            darwin::CreateBasicElement(
                "ground",
                proto::TYPE_GROUND,
                darwin::CreateVector3(-10.0, 20.0, -30.0),
                1'000'000'000.0,
                10.0));
        // Tiny spec in space!
        world_state_->AddElement(
            0.0,
            darwin::CreateBasicElement(
                "elem",
                proto::TYPE_BROWN,
                darwin::CreateVector3(0, 0, 0),
                1.0,
                0.1));
        double height = std::numeric_limits<double>::max();
        double time = 1.0;
        for (int i = 0; i < 10'000; ++i) {
            world_state_->Update(time++);
            auto elements = world_state_->GetElements();
            EXPECT_EQ(elements.size(), 2);
            double distance = glm::distance(
                darwin::ProtoVector2Glm(elements[1].physic().position()),
                darwin::ProtoVector2Glm(elements[0].physic().position()));
            EXPECT_LE(distance, height + 0.01);
            height = (distance < 10.1) ? 10.1 : distance;
        }
        EXPECT_NEAR(height, 10.1, 0.01);
    }

    TEST_F(WorldStateTest, WorldStateTestGroundAndCharacter) {
        EXPECT_FALSE(world_state_);
        world_state_ = std::make_unique<darwin::WorldState>();
        EXPECT_TRUE(world_state_);
        // A planet with the mass of 1'000'000'000.0g (1'000'000kg) and a
        // radius of 10.0.
        world_state_->AddElement(
            0.0,
            darwin::CreateBasicElement(
                "ground",
                proto::TYPE_GROUND,
                darwin::CreateVector3(-10.0, 20.0, -30.0),
                1'000'000'000.0,
                10.0));
        // a character with a mass of 80'000.0g (80kg) and a radius of 1.0.
        world_state_->AddCharacter(
            0.0, 
            darwin::CreateBasicCharacter(
                "character",
                darwin::CreateVector3(1.0, -4.0, 2.0),
                80'000.0,
                1.0));
        double height = std::numeric_limits<double>::max();
        double time = 1.0;
        for (int i = 0; i < 10'000; ++i) {
            world_state_->Update(time++);
            auto characters = world_state_->GetCharacters();
            auto elements = world_state_->GetElements();
            EXPECT_EQ(elements.size(), 1);
            EXPECT_EQ(characters.size(), 1);
            auto distance = glm::distance(
                darwin::ProtoVector2Glm(characters[0].physic().position()), 
                darwin::ProtoVector2Glm(elements[0].physic().position()));
            EXPECT_LE(distance, height + 0.01);
            height = (distance < 11.0) ? 11.0 : distance;
        }
        EXPECT_NEAR(height, 11.0, 0.01);
    }

}  // namespace test.