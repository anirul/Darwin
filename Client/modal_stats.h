#pragma once

#include <string>
#include <vector>

#include "frame/gui/gui_window_interface.h"
#include "Common/world_simulator.h"

namespace darwin::modal {

    enum class ModalStatsButton {
        None,
        Close,
    };

    struct ModalStatsParams {
        ModalStatsButton button_result = ModalStatsButton::None;
        bool show_close_button = false;
    };

    class ModalStats : public frame::gui::GuiWindowInterface {
    public:
        ModalStats(
            const std::string& name, 
            ModalStatsParams& params);
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;

    public:
        void SetCharacters(const std::vector<proto::Character>& characters) {
            characters_ = characters;
        }

    private:
        std::string name_;
        ModalStatsParams& params_;
        bool end_ = false;
        std::vector<proto::Character> characters_;
    };

} // namespace darwin::modal.