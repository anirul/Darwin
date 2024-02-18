#include "user_state_file.h"

#include "Common/darwin_service.grpc.pb.h"
#include "Common/stl_proto_wrapper.h"

namespace darwin {

    void SaveUserStateToString(std::string& json, const UserState& user_state) {
        proto::UserDatabase users;
        for (const auto feature : user_state.GetFeatures()) {
            users.add_features(feature);
        }
        for (const auto& user : user_state.GetUsers()) {
            users.add_users()->CopyFrom(user);
        }
        json = SaveProtoToJson(users);
    }

    void LoadUserStateFromString(
        UserState& user_state, 
        const std::string& json) {
        auto users = LoadProtoFromJson<proto::UserDatabase>(json);
        for (const auto feature : users.features()) {
            user_state.AddFeature(
                static_cast<proto::UserDatabase::ParameterEnum>(feature));
        }
        for (const auto& user : users.users()) {
            user_state.AddUser(user.name(), user.password(), user.salt());
        }
    }

    void LoadUserStateFromFile(
        UserState& user_state,
        const std::filesystem::path file_path) {
        auto users = LoadProtoFromJsonFile<proto::UserDatabase>(file_path);
        for (const auto feature : users.features()) {
            user_state.AddFeature(
                static_cast<proto::UserDatabase::ParameterEnum>(feature));
        }
        for (const auto& user : users.users()) {
            user_state.AddUser(user.name(), user.password(), user.salt());
        }
    }

    void SaveUserStateToFile(
        const std::filesystem::path file_path, 
        const UserState& user_state) {
        proto::UserDatabase users;
        for (const auto feature : user_state.GetFeatures()) {
            users.add_features(feature);
        }
        for (const auto& user : user_state.GetUsers()) {
            users.add_users()->CopyFrom(user);
        }
        SaveProtoToJsonFile(users, file_path);
    }

}  // namespace darwin.
