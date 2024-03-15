#pragma once

#include <string>

#include "frame/gui/gui_window_interface.h"

namespace darwin::modal {

    enum class ModalVictoryButton {
        None,
        Respawn,
        Exit,
    };

    struct ModalVictoryParams {
        ModalVictoryButton button_result = ModalVictoryButton::None;
    };

    class ModalVictory : public frame::gui::GuiWindowInterface {
    public:
        ModalVictory(const std::string& name, ModalVictoryParams& params);
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;

    private:
        std::string name_;
        ModalVictoryParams& params_;
        bool end_ = false;
    };

} // namespace darwin::modal.