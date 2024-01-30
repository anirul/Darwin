#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>

#include "world_client.h"
#include "Common/darwin_service.pb.h"
#include "Common/darwin_service.grpc.pb.h"

namespace darwin {

    class DarwinClient {
    public:
        DarwinClient(const std::string& name, std::shared_ptr<grpc::Channel> channel);
        void Push(const proto::Physic& physic);
        void Update(WorldClient& world_client);
        bool IsConnected() const;

    private:
        std::string name_;
        std::unique_ptr<proto::DarwinService::Stub> stub_;
    };

} // namespace darwin.
