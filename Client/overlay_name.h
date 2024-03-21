#pragma once

#include <string>
#include <vector>

#include "frame/gui/gui_window_interface.h"
#include "Common/world_simulator.h"

namespace darwin::overlay {

    class OverlayName : public frame::gui::GuiWindowInterface {
    public:
        OverlayName(const std::string& name);
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;
    };

} // namespace darwin::overlay.
