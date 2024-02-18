#include "Common/darwin_service.grpc.pb.h"
#include "Common/stl_proto_wrapper.h"
#include "world_state_file.h"

#include <fstream>

namespace darwin {

    void SaveWorldStateToString(std::string& json, const WorldState& world_state)
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
            world_state.AddCharacter(time, character);
        }
        for (const auto& element : world.elements())
        {
            world_state.AddElement(time, element);
        }
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
            world_state.AddCharacter(time, character);
        }
        for (const auto& element : world.elements())
        {
            world_state.AddElement(time, element);
        }
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
        SaveProtoToJsonFile(world, filename);
    }

} // namespace darwin.
