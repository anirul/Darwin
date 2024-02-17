#pragma once

#include <memory>

#include "frame/gui/gui_window_interface.h"

namespace darwin::modal {

    enum class ModalDisconnectedButton {
        None,
        Retry,
        Cancel,
    };

    struct ModalDisconnectedParams {
        ModalDisconnectedButton button_result = ModalDisconnectedButton::None;
    };

    class ModalDisconnected : public frame::gui::GuiWindowInterface {
    public:
        ModalDisconnected(
            const std::string& name, 
            ModalDisconnectedParams& params);
        ~ModalDisconnected() override;
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;

    private:
        std::string name_;
        ModalDisconnectedParams& params_;
    };

} // namespace darwin::modal.
