#pragma once

#include "Common/darwin_service.pb.h"

namespace darwin {

    class WorldClient {
    public:
        std::vector<proto::Element> GetElements() const;
        std::vector<proto::Player> GetPlayers() const;
        void SetElements(const std::vector<proto::Element>& elements);
        void SetPlayers(const std::vector<proto::Player>& players);

    private:
        mutable std::mutex mutex_;
        std::vector<proto::Element> elements_;
        std::vector<proto::Player> players_;
    };

} // namespace darwin.