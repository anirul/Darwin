#pragma once

#include "frame/gui/gui_window_interface.h"

namespace darwin::modal {

    struct ModalServerParams {
        std::string server_name;
        bool succeeded = false;
    };

    class ModalServer : public frame::gui::GuiWindowInterface {
    public:
        ModalServer(const std::string& name, ModalServerParams& params) :
            name_(name), params_(params) {}
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;

    private:
        std::string name_;
        ModalServerParams& params_;
        bool end_ = false;
    };

} // End namespace frame::modal.
