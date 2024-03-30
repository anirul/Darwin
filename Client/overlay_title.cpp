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
        start_time_ = std::chrono::system_clock::now();
    }

    bool OverlayTitle::DrawCallback() {
        auto duration = std::chrono::system_clock::now() - start_time_;
        overlay_draw_.Parameter(
            "timer_s", 
            std::chrono::duration<double>(duration).count());
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
