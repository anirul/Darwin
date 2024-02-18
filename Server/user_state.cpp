#include "user_state.h"

namespace darwin {

    std::int32_t UserState::AddUser(
        const std::string& user_name, 
        const std::string& password, 
        const std::string& salt) {
        // Lock to prevent multiple inclusion of the same user.
        std::lock_guard<std::mutex> lock(mutex_);
        // Already exists.
        if (user_ids_.find(user_name) != user_ids_.end()) {
            return -1;
        }
        proto::User user;
        user.set_name(user_name);
        user.set_password(password);
        user.set_salt(salt);
        static std::int32_t next_id = 1;
        user.set_user_id(next_id++);
        auto id = user.user_id();
        user_ids_.insert({user_name, id});
        id_users_.insert({id, std::move(user)});
        return id;
    }

    bool UserState::CheckUser(
        const std::string& user_name, 
        const std::string& password) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = user_ids_.find(user_name);
        if (it == user_ids_.end()) {
            return false;
        }
        auto id = it->second;
        auto it2 = id_users_.find(id);
        if (it2 == id_users_.end()) {
            return false;
        }
        auto& user = it2->second;
        if (user.password() != password) {
            return false;
        }
        return true;
    }

    std::string UserState::GetSalt(const std::string& user_name) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = user_ids_.find(user_name);
        if (it == user_ids_.end()) {
            return "";
        }
        auto id = it->second;
        auto it2 = id_users_.find(id);
        if (it2 == id_users_.end()) {
            return "";
        }
        auto& user = it2->second;
        return user.salt();
    }

    const std::vector<proto::User>& UserState::GetUsers() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<proto::User> users;
        for (auto& pair : id_users_) {
            users.push_back(pair.second);
        }
        return users;
    }

}  // namespace darwin.
