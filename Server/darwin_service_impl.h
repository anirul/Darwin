#pragma once

#include <grpc++/grpc++.h>

#include "Common/darwin_service.grpc.pb.h"
#include "world_state.h"

namespace darwin {

    class DarwinServiceImpl final : public proto::DarwinService::Service {
    public:
        DarwinServiceImpl(WorldState& world_state) : 
            world_state_(world_state) {}

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
        void ComputeWorld();
        // Returns the name of character against the potential hits this will
        // empty the list after the call.
        std::map<std::string, std::string> GetPotentialHits();

    protected:
        proto::Physic UpdatePhysic(
            const proto::Physic& server_physic, 
            const proto::Physic& client_physic) const;
        
    protected:
        std::map<double, proto::Character> time_characters_;
        // Name of the character against name of potential hits.
        std::map<std::string, std::string> character_potential_hits_;
        WorldState& world_state_;
        std::list<grpc::ServerWriter<proto::UpdateResponse>*> writers_;
        std::mutex writers_mutex_;
    };

}  // namespace darwin.
