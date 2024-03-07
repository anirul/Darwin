#pragma once

#include <grpc++/grpc++.h>
#include <memory>

#include "Common/darwin_service.grpc.pb.h"

namespace darwin {

    class UpdateResponder;

    class UpdateResponderContainer {
    public:
        void AddResponder(
            std::unique_ptr<UpdateResponder> responder);
        void RemoveResponder(UpdateResponder* responder);
        void BroadcastUpdate(const proto::UpdateResponse& response);

    private:
        std::mutex mutex_;
        std::list<std::unique_ptr<UpdateResponder>> update_responders_;
    };

} // namespace darwin.
