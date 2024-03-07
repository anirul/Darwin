#include "update_responder_container.h"

#include "update_responder.h"

namespace darwin {

    void UpdateResponderContainer::AddResponder(
        std::unique_ptr<UpdateResponder> responder) 
    {
        std::scoped_lock lock(mutex_);
        update_responders_.push_back(std::move(responder));
    }

    void UpdateResponderContainer::RemoveResponder(
        UpdateResponder* responder) 
    {
        std::scoped_lock lock(mutex_);
        update_responders_.remove_if(
            [responder](const std::unique_ptr<UpdateResponder>& r) {
                return r.get() == responder;
            });
    }

    void UpdateResponderContainer::BroadcastUpdate(
        const proto::UpdateResponse& response) 
    {
        std::scoped_lock lock(mutex_);
        for (auto& responder : update_responders_) {
            responder->BroadcastUpdate(response);
        }
    }

} // namespace darwin.
