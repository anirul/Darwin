#pragma once

#include <filesystem>

#include "user_state.h"

namespace darwin {

    void SaveUserStateToString(
        std::string& json, 
        const UserState& user_state);

    void LoadUserStateFromString(
        UserState& user_state, 
        const std::string& json);

    void SaveUserStateToFile(
        const std::filesystem::path file_path,
        const UserState& user_state);

    void LoadUserStateFromFile(
        UserState& user_state,
        const std::filesystem::path file_path);

} // namespace darwin.
