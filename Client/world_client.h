#pragma once

#include "Common/darwin_service.pb.h"
#include "frame/logger.h"

namespace darwin {

    class WorldClient {
    public:
        std::vector<proto::Element> GetElements() const;
        std::vector<proto::Character> GetCharacters() const;
        void SetElements(const std::vector<proto::Element>& elements);
        void SetCharacters(const std::vector<proto::Character>& characters);

    private:
        mutable std::mutex mutex_;
        std::vector<proto::Element> elements_;
        std::vector<proto::Character> characters_;
        frame::Logger& logger_ = frame::Logger::GetInstance();
    };

} // namespace darwin.