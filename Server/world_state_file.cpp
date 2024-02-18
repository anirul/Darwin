#include "Common/darwin_service.grpc.pb.h"
#include "Common/stl_proto_wrapper.h"
#include "world_state_file.h"

#include <fstream>

namespace darwin {

    void SaveWorldStateToString(std::string& json, const WorldState& world_state)
    {
        proto::World world;
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
        proto::World world = LoadProtoFromJson<proto::World>(str);
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
        proto::World word = LoadProtoFromJsonFile<proto::World>(filename);
        double time = word.time();
        for (const auto& character : word.characters())
        {
            world_state.AddCharacter(time, character);
        }
        for (const auto& element : word.elements())
        {
            world_state.AddElement(time, element);
        }
        world_state.Update(time);
    }

    void SaveWorldStateToFile(
        const WorldState& world_state,
        const std::filesystem::path filename)
    {
        proto::World word;
        word.set_time(world_state.GetLastUpdated());
        for (const auto& character : world_state.GetCharacters())
        {
            *word.add_characters() = character;
        }
        for (const auto& element : world_state.GetElements())
        {
            *word.add_elements() = element;
        }
        SaveProtoToJsonFile(word, filename);
    }

} // namespace darwin.
