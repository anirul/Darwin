#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>

#include "world_client.h"
#include "Common/darwin_service.pb.h"
#include "Common/darwin_service.grpc.pb.h"
#include "frame/logger.h"

namespace darwin {

    class DarwinClient {
    public:
        DarwinClient(const std::string& name, std::shared_ptr<grpc::Channel> channel);
        void ReportMovement(const proto::Physic& physic);
        void Update(WorldClient& world_client);
        std::int32_t Ping(std::int32_t val = 45323);
        bool IsConnected() const;

    private:
        std::string name_;
        double server_time_ = 0.0;
        std::unique_ptr<proto::DarwinService::Stub> stub_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
    };

} // namespace darwin.
