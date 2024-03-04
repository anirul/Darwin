#include "Common/darwin_service.grpc.pb.h"
#include "Common/stl_proto_wrapper.h"
#include "world_state_file.h"

#include <fstream>

namespace darwin {

    void SaveWorldStateToString(
        std::string& json, 
        const WorldState& world_state)
    {
        proto::WorldDatabase world;
        world.set_time(world_state.GetLastUpdated());
        for (const auto& character : world_state.GetCharacters())
        {
            *world.add_characters() = character;
        }
        for (const auto& element : world_state.GetElements())
        {
            *world.add_elements() = element;
        }
        world.mutable_player_parameter()->CopyFrom(
            world_state.GetPlayerParameter());
        json = SaveProtoToJson(world);
    }

    void LoadWorldStateFromString(
        WorldState& world_state, 
        const std::string& str)
    {
        auto world = LoadProtoFromJson<proto::WorldDatabase>(str);
        double time = world.time();
        for (const auto& character : world.characters())
        {
            // This should never happen as there is no character that is not
            // controled by a human.
            // world_state.AddCharacter(time, character);
        }
        for (const auto& element : world.elements())
        {
            world_state.AddElement(time, element);
        }
        world_state.SetPlayerParameter(world.player_parameter());
        world_state.Update(time);
    }

    void LoadWorldStateFromFile(
        WorldState& world_state,
        const std::filesystem::path filename)
    {
        auto world = LoadProtoFromJsonFile<proto::WorldDatabase>(filename);
        double time = world.time();
        for (const auto& character : world.characters())
        {
            // This should not happen as there is no character that is not 
            // control by a human.
            // world_state.AddCharacter(time, character);
        }
        for (const auto& element : world.elements())
        {
            world_state.AddElement(time, element);
        }
        world_state.SetPlayerParameter(world.player_parameter());
        world_state.Update(time);
    }

    void SaveWorldStateToFile(
        const WorldState& world_state,
        const std::filesystem::path filename)
    {
        proto::WorldDatabase world;
        world.set_time(world_state.GetLastUpdated());
        for (const auto& character : world_state.GetCharacters())
        {
            *world.add_characters() = character;
        }
        for (const auto& element : world_state.GetElements())
        {
            *world.add_elements() = element;
        }
        world.mutable_player_parameter()->CopyFrom(
            world_state.GetPlayerParameter());
        SaveProtoToJsonFile(world, filename);
    }

} // namespace darwin.
