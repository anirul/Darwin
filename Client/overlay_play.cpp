#include "overlay_play.h"

namespace darwin::overlay {

    OverlayPlay::OverlayPlay(
        const std::string& name,
        const proto::PageDescription& page_description)
        : name_(name),
          page_description_(page_description) 
    {
    }

    bool OverlayPlay::DrawCallback() {
        return true;
    }

    bool OverlayPlay::End() const {
        return false;
    }

    void OverlayPlay::SetName(const std::string& name) {
        name_ = name;
    }

    std::string OverlayPlay::GetName() const {
        return name_;
    }

} // namespace darwin::overlay.
