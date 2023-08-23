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
        void BroadcastUpdate(const proto::UpdateResponse& response);

    protected:
        std::list<grpc::ServerWriter<proto::UpdateResponse>*> writers_;
        std::mutex writers_mutex_;
        std::vector<proto::Element> elements_;
        std::vector<proto::Player> players_;
    };

}  // namespace darwin.
