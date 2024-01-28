#pragma once

#include <filesystem>

#include "world_state.h"

namespace darwin {

    void SaveWorldStateToString(
        std::string& json, 
        const WorldState& world_state);

    void LoadWorldStateFromString(
        WorldState& world_state,
        const std::string& str);

    void SaveWorldStateToFile(
        const WorldState& world_state, 
        const std::filesystem::path filename);

    void LoadWorldStateFromFile(
        WorldState& world_state, 
        const std::filesystem::path filename);

} // namespace darwin
