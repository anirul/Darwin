#pragma once

#include <string>

#include "frame/gui/gui_window_interface.h"

namespace darwin::modal {

    enum class ModalDeathButton {
        None,
        Respawn,
        Exit,
    };

    struct ModalDeathParams {
        ModalDeathButton button_result = ModalDeathButton::None;
    };

    class ModalDeath : public frame::gui::GuiWindowInterface {
    public:
        ModalDeath(const std::string& name, ModalDeathParams& params);
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;

    private:
        std::string name_;
        ModalDeathParams& params_;
        bool end_ = false;
    };

} // namespace darwin::modal.