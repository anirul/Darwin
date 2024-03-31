#pragma once

#include <grpc++/grpc++.h>

#include "Common/darwin_service.grpc.pb.h"
#include "Common/stl_proto_wrapper.h"
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
        grpc::Status ReportInGame(
            grpc::ServerContext* context, 
            const proto::ReportInGameRequest* request,
            proto::ReportInGameResponse* response) override;
        grpc::Status CreateCharacter(
            grpc::ServerContext* context, 
            const proto::CreateCharacterRequest* request,
            proto::CreateCharacterResponse* response) override;
        grpc::Status Ping(
            grpc::ServerContext* context, 
            const proto::PingRequest* request,
            proto::PingResponse* response) override;

    public:
        std::vector<proto::Character>& GetCharacters();
        void ClearCharacters();
        void ComputeWorld(double loop_timer);

    protected:
        void BroadcastUpdateLocked(const proto::UpdateResponse& response);
        proto::SpecialEffectParameter UpdateSpecialEffectBoost(
            const proto::SpecialEffectParameter& special_effect,
            double delta_time) const;
        proto::Physic UpdatePhysic(
            const proto::Physic& server_physic, 
            const proto::Physic& client_physic) const;
        void CheckNewBoost(
            proto::Character& character,
            const proto::SpecialEffectParameter& new_special_effect);
        
    protected:
        std::vector<proto::Character> characters_;
        // Name of the character against name of potential hits.
        std::map<proto::Character, std::string> character_hits_;
        WorldState& world_state_;
        std::list<grpc::ServerWriter<proto::UpdateResponse>*> writers_;
        std::mutex writers_mutex_;
        double loop_timer_ = 0.0;
    };

}  // namespace darwin.
