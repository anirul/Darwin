#pragma once

#include <future>
#include <grpc++/grpc++.h>
#include "Common/darwin_service.grpc.pb.h"

namespace darwin {

    struct AsyncClientCall {
        grpc::ClientContext context;
        grpc::Status status;
        proto::ReportMovementRequest request;
        std::shared_ptr<proto::ReportMovementResponse> response;
        std::shared_ptr<std::promise<proto::ReportMovementResponse>> promise;
        std::unique_ptr<
            grpc::ClientAsyncResponseReader<proto::ReportMovementResponse>> 
            rpc;
    };

} // namespace darwin.
