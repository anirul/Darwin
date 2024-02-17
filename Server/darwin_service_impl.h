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
        grpc::Status Push(
            grpc::ServerContext* context, 
            const proto::PushRequest* request,
            proto::PushResponse* response) override;
        grpc::Status Ping(
            grpc::ServerContext* context, 
            const proto::PingRequest* request,
            proto::PingResponse* response) override;
        void BroadcastUpdate(const proto::UpdateResponse& response);
        std::map<double, proto::Player>& GetTimePlayers();
        void ClearTimePlayers();
        std::mutex& GetTimePLayersMutex();

    protected:
        std::map<double, proto::Player> time_players_;
        std::list<grpc::ServerWriter<proto::UpdateResponse>*> writers_;
        std::mutex writers_mutex_;
        std::vector<proto::Element> elements_;
        std::vector<proto::Player> players_;
    };

}  // namespace darwin.
