#pragma once

#include "frame/gui/gui_window_interface.h"

namespace darwin::modal {

    enum class ModalServerButton {
        None,
        Connect,
        Cancel,
    };

    struct ModalServerParams {
        std::string server_name;
        ModalServerButton button_result = ModalServerButton::None;
    };

    class ModalServer : public frame::gui::GuiWindowInterface {
    public:
        ModalServer(const std::string& name, ModalServerParams& params);
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;

    private:
        std::string name_;
        ModalServerParams& params_;
        bool end_ = false;
        char server_[1024] = { 0 };
    };

} // End namespace frame::modal.
