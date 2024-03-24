#include "overlay_state.h"

#include "overlay_font.h"

namespace darwin::overlay {

    OverlayState::OverlayState(
        const std::string name,
        const proto::ClientParameter& client_parameter,
        const proto::PageDescription& page_description)
        : name_(name),
          page_description_(page_description),
          overlay_draw_(client_parameter, page_description)
    {
        OverlayFont::GetInstance(client_parameter);
    }

    bool OverlayState::DrawCallback() {
        overlay_draw_.Parameter("state_name", state_name_);
        return overlay_draw_.Draw();
    }

    bool OverlayState::End() const {
        return false;
    }

    std::string OverlayState::GetName() const {
        return name_;
    }

    void OverlayState::SetName(const std::string& name) {
        name_ = name;
    }

} // namespace darwin::overlay.
