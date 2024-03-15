#include "world_state_file_test.h"
#include "Common/stl_proto_wrapper.h"
#include "Common/vector.h"

namespace test {

    void WorldStateFileTest::PopulateWorldState() {
        world_state_.AddElement(
            0.0,
            darwin::CreateBasicElement(
                "element1",
                proto::TYPE_GROUND,
                darwin::CreateVector3(1.0, 2.0, 3.0),
                1.0,
                1.0));
        world_state_.AddElement(
            0.0,
            darwin::CreateBasicElement(
                "element2",
                proto::TYPE_GROUND,
                darwin::CreateVector3(4.0, 5.0, 6.0),
                2.0,
                2.0));
        world_state_.AddCharacter(
            0.0,
            darwin::CreateBasicCharacter(
                "character1",
                darwin::CreateVector3(7.0, 8.0, 9.0),
                3.0,
                3.0));
        world_state_.AddCharacter(
            0.0,
            darwin::CreateBasicCharacter(
                "character2",
                darwin::CreateVector3(10.0, 11.0, 12.0),
                4.0,
                4.0));
    }

    TEST_F(WorldStateFileTest, LoadWorldStateFileTestNoFile) {
        darwin::WorldState world_state;
        EXPECT_THROW(
            darwin::LoadWorldStateFromFile(
                world_state, 
                std::filesystem::path("test.json")),
            std::exception);
    }

    TEST_F(WorldStateFileTest, LoadAndSaveFromLocalProto) {
        PopulateWorldState();
        // Forced to copy from map to characters/elements vectors.
        world_state_.Update(0.0);
        std::string json;
        EXPECT_NO_THROW(
            darwin::SaveWorldStateToString(
                json, 
                world_state_));
        darwin::WorldState world_state;
        EXPECT_NO_THROW(
            darwin::LoadWorldStateFromString(
                world_state, 
                json));
        // Forced to copy from map to characters/elements vectors.
        world_state.Update(0.0);
        EXPECT_EQ(world_state_, world_state);
    }

} // namespace test.
