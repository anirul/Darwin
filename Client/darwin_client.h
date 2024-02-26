#pragma once

#include <future>
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
        DarwinClient(const std::string& name);
        ~DarwinClient();
        bool CreateCharacter(
            const std::string& name, 
            const proto::Vector3& color);
        void ReportMovement(const proto::Physic& physic);
        void Update(WorldClient& world_client);
        std::int32_t Ping(std::int32_t val = 45323);
        bool IsConnected() const;

    public:
        std::vector<proto::Element> GetElements() const {
            return world_client_.GetElements();
        }
        std::vector<proto::Character> GetCharacters() const {
            return world_client_.GetCharacters();
        }
        double GetServerTime() const {
            return server_time_;
        }
        std::string GetCharacterName() const {
            return character_name_;
        }

    private:
        std::string name_;
        std::string character_name_;
        double server_time_ = 0.0;
        std::unique_ptr<proto::DarwinService::Stub> stub_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
        WorldClient world_client_;
        std::future<void> future_;
        std::atomic<bool> end_{ false };
    };

} // namespace darwin.
