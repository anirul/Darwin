#include "world_client.h"

namespace darwin {

    std::vector<proto::Element> WorldClient::GetElements() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return elements_;
    }

    std::vector<proto::Character> WorldClient::GetCharacters() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return characters_;
    }

    void WorldClient::SetElements(
        const std::vector<proto::Element>& elements) {
        std::lock_guard<std::mutex> lock(mutex_);
        elements_ = elements;
    }

    void WorldClient::SetCharacters(
        const std::vector<proto::Character>& characters) {
        std::lock_guard<std::mutex> lock(mutex_);
        characters_ = characters;
    }

} // namespace darwin.
