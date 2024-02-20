#pragma once

#include "Common/darwin_service.grpc.pb.h"

#include <grpc++/grpc++.h>

namespace darwin {

    class DarwinServiceImpl final : public proto::DarwinService::Service {
    public:
        grpc::Status Update(
            grpc::ServerContext* context, 
            const proto::UpdateRequest* request,
            grpc::ServerWriter<proto::UpdateResponse>* writer) override;
        grpc::Status ReportMovement(
            grpc::ServerContext* context, 
            const proto::ReportMovementRequest* request,
            proto::ReportMovementResponse* response) override;
        grpc::Status CreateCharacter(
            grpc::ServerContext* context, 
            const proto::CreateCharacterRequest* request,
            proto::CreateCharacterResponse* response) override;
        grpc::Status Ping(
            grpc::ServerContext* context, 
            const proto::PingRequest* request,
            proto::PingResponse* response) override;
        grpc::Status DeathReport(
            grpc::ServerContext* context, 
            const proto::DeathReportRequest* request,
            proto::DeathReportResponse* response) override;

    public:
        void BroadcastUpdate(const proto::UpdateResponse& response);
        std::map<double, proto::Character>& GetTimeCharacters();
        void ClearTimeCharacters();
        std::mutex& GetTimeCharacterMutex();

    protected:
        std::map<double, proto::Character> time_characters_;
        std::list<grpc::ServerWriter<proto::UpdateResponse>*> writers_;
        std::mutex writers_mutex_;
    };

}  // namespace darwin.
