#include "overlay_title.h"

namespace darwin::overlay {

    OverlayTitle::OverlayTitle(
        const std::string& name,
        const proto::ClientParameter& client_parameter,
        const proto::PageDescription& page_description)
        : name_(name),
          page_description_(page_description),
          overlay_draw_(client_parameter, page_description)
    {
    }

    bool OverlayTitle::DrawCallback() {
        overlay_draw_.Draw();
        return true;
    }

    bool OverlayTitle::End() const {
        return false;
    }

    std::string OverlayTitle::GetName() const {
        return name_;
    }

    void OverlayTitle::SetName(const std::string& name) {
        name_ = name;
    }

} // namespace darwin::overlay.
