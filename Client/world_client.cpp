#include "world_client.h"

namespace darwin {

    std::vector<proto::Element> WorldClient::GetElements() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return elements_;
    }

    std::vector<proto::Player> WorldClient::GetPlayers() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return players_;
    }

    void WorldClient::SetElements(const std::vector<proto::Element>& elements) {
        std::lock_guard<std::mutex> lock(mutex_);
        elements_ = elements;
    }

    void WorldClient::SetPlayers(const std::vector<proto::Player>& players) {
        std::lock_guard<std::mutex> lock(mutex_);
        players_ = players;
    }

} // namespace darwin.
