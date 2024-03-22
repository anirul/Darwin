#pragma once

#include <string>

#include "frame/gui/gui_window_interface.h"
#include "Common/client_parameter.pb.h"
#include "overlay_draw.h"

namespace darwin::overlay {

    class OverlayTitle : public frame::gui::GuiWindowInterface {
    public:
        OverlayTitle(
            const std::string& name,
            const proto::ClientParameter& client_parameter,
            const proto::PageDescription& page_description);
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;

    private:
        std::string name_;
        proto::PageDescription page_description_;
        OverlayDraw overlay_draw_;
    };

} // namespace darwin::overlay.
