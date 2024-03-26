#pragma once

#include <string>
#include <vector>

#include "frame/gui/gui_window_interface.h"
#include "Common/client_parameter.pb.h"
#include "overlay_draw.h"

namespace darwin::overlay {

    class OverlayState : public frame::gui::GuiWindowInterface {
    public:
        OverlayState(
            const std::string name,
            const proto::ClientParameter& client_parameter,
            const proto::PageDescription& page_description);
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;

    public:
        void SetStateName(const std::string& state_name) {
            state_name_ = state_name;
        }

    private:
        std::string name_;
        std::string state_name_;
        proto::PageDescription page_description_;
        OverlayDraw overlay_draw_;
    };

} // namespace darwin::overlay.