#pragma once

#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>

#include "Common/world_simulator.h"
#include "Common/darwin_constant.h"
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
        void ReportMovement(
            const proto::Physic& physic,
            const std::string& potential_hit);
        void ReportPing();
        void SendReportInGame();
        void Update();
        std::int32_t Ping(std::int32_t val = 45323);
        bool IsConnected() const;
        proto::Character MergeCharacter(proto::Character new_characters);
        std::vector<proto::ColorParameter> GetColorParameters() const;

    public:
        WorldSimulator& GetWorldSimulator() {
            return world_simulator_;
        }
        std::vector<proto::Element> GetElements() const {
            return world_simulator_.GetElements();
        }
        std::vector<proto::Character> GetCharacters() const {
            return world_simulator_.GetCharacters();
        }
        double GetServerTime() const {
            return server_time_.load();
        }
        std::string GetCharacterName() const {
            return character_name_;
        }

    protected:
        void SendReportInGameSync();
        proto::Character CorrectCharacter(
            const proto::Character& server_character,
            const proto::Character& client_character) const;
        void Clear();

    private:
        mutable std::mutex mutex_;
        proto::ReportInGameRequest report_request_;
        std::string name_;
        std::string character_name_;
        std::atomic<double> server_time_ = 0.0;
        std::unique_ptr<proto::DarwinService::Stub> stub_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
        WorldSimulator world_simulator_;
        std::future<void> update_future_;
        std::atomic<bool> end_{ false };
        std::shared_ptr<grpc::ClientContext> context_ = 
            std::make_shared<grpc::ClientContext>();
    };

} // namespace darwin.
