#pragma once

#include <grpc++/grpc++.h>
#include <memory>

#include "Common/darwin_service.grpc.pb.h"
#include "world_state.h"
#include "update_responder.h"
#include "update_responder_container.h"

namespace darwin {

    class DarwinServiceImpl final : public proto::DarwinService::AsyncService 
    {
    public:
        explicit DarwinServiceImpl(
            grpc::ServerCompletionQueue* cq,
            WorldState& world_state) : 
            cq_(cq), world_state_(world_state)
        {
            ListenForUpdates();
        }
        ~DarwinServiceImpl() {
            cq_->Shutdown();
        }
        void ListenForUpdates() {
            update_responders_.AddResponder(
                std::make_unique<UpdateResponder>(
                    this, 
                    update_responders_, 
                    cq_));
        }

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
        std::map<double, proto::Character>& GetTimeCharacters();
        void ClearTimeCharacters();
        void ComputeWorld();
        // Returns the name of character against the potential hits this will
        // empty the list after the call.
        std::map<std::string, std::string> GetPotentialHits();

    protected:
        proto::Physic UpdatePhysic(
            const proto::Physic& server_physic, 
            const proto::Physic& client_physic) const;
        
    protected:
        UpdateResponderContainer update_responders_;
        grpc::ServerCompletionQueue* cq_;
        std::map<double, proto::Character> time_characters_;
        // Name of the character against name of potential hits.
        std::mutex mutex_;
        std::map<std::string, std::string> character_potential_hits_;
        WorldState& world_state_;
    };

}  // namespace darwin.
