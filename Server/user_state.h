#pragma once

#include "Common/darwin_service.grpc.pb.h"

namespace darwin {

    class UserState {
    public:
        std::int32_t AddUser(
            const std::string& user_name, 
            const std::string& password, 
            const std::string& salt);
        bool CheckUser(
            const std::string& user_name, 
            const std::string& password);
        std::string GetSalt(const std::string& user_name);
        const std::vector<proto::User>& GetUsers();

    private:
        std::mutex mutex_;
        std::map<std::string, std::int32_t> user_ids_;
        std::map<std::int32_t, proto::User> id_users_;    };

} // namespace darwin.
