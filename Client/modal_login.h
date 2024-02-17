#pragma once

#include "frame/gui/gui_window_interface.h"

namespace darwin::modal {

    enum class ModalLoginButton {
        None,
        Login,
        Cancel,
    };
    
    struct ModalLoginParams {
        std::string username;
        std::string password;
        ModalLoginButton button_result = ModalLoginButton::None;
    };

    class ModalLogin : public frame::gui::GuiWindowInterface {
    public:
        ModalLogin(const std::string& name, ModalLoginParams& params) : 
            name_(name), params_(params) {}
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;

    private:
        bool end_ = false;
        std::string name_;
        ModalLoginParams& params_;
    };

} // namespace darwin::modal.
