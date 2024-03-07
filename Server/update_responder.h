#pragma once

#include <grpc++/grpc++.h>
#include <memory>

#include "Common/darwin_service.grpc.pb.h"

namespace darwin {

class UpdateResponderContainer;

class UpdateResponder {
public:
    UpdateResponder(
        proto::DarwinService::AsyncService* service, 
        UpdateResponderContainer& container,
        grpc::ServerCompletionQueue* cq);
    void Proceed(bool ok);
    void BroadcastUpdate(const proto::UpdateResponse& response);

private:
    proto::DarwinService::AsyncService* service_;
    grpc::ServerCompletionQueue* cq_;
    grpc::ServerContext ctx_;
    proto::UpdateRequest request_;
    grpc::ServerAsyncWriter<proto::UpdateResponse> responder_;
    UpdateResponderContainer& container_;
    bool writing_ = false;
};

}  // namespace darwin.
