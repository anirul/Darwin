#include "update_responder.h"

#include "update_responder_container.h"

namespace darwin {

    UpdateResponder::UpdateResponder(
        proto::DarwinService::AsyncService* service,
        UpdateResponderContainer& container,
        grpc::ServerCompletionQueue* cq)
        : service_(service), cq_(cq), responder_(&ctx_), container_(container)
    {
        // Immediately request to start handling Update RPCs
        service_->RequestUpdate(&ctx_, &request_, &responder_, cq_, cq_, this);
    }

    void UpdateResponder::Proceed(bool ok) {
        if (!ok) {
            container_.RemoveResponder(this);
            return;
        }

        if (writing_) {
            writing_ = false;
        }
    }

    void UpdateResponder::BroadcastUpdate(const proto::UpdateResponse& response) {
        writing_ = true;
        responder_.Write(response, this);
    }

}