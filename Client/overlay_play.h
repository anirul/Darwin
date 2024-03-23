#pragma once

#include <string>
#include <vector>

#include "frame/gui/gui_window_interface.h"
#include "Common/world_simulator.h"
#include "Common/client_parameter.pb.h"
#include "Common/darwin_service.pb.h"
#include "overlay_draw.h"

namespace darwin::overlay {

    class OverlayPlay : public frame::gui::GuiWindowInterface {
    public:
        OverlayPlay(
            const std::string& name,
            const proto::ClientParameter& client_parameter,
            const proto::PageDescription& page_description);
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;

    public:
        void SetCharacters(const std::vector<proto::Character>& characters) {
            characters_ = characters;
        }
        void SetCharacterName(const std::string& name) {
            character_name_ = name;
        }

    private:
        std::string name_;
        std::string character_name_;
        proto::PageDescription page_description_;
        std::vector<proto::Character> characters_;
        OverlayDraw overlay_draw_;
    };

} // namespace darwin::overlay.
