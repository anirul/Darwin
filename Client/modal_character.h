#pragma once

#include <string>
#include <memory>

#include "Common/darwin_service.pb.h"
#include "frame/gui/gui_window_interface.h"

namespace darwin::modal {

    enum class ModalCharacterButton {
        None,
        Create,
        Cancel,
    };

    struct ModalCharacterParams {
        std::string name = "";
        proto::Vector3 color;
        ModalCharacterButton button_result = ModalCharacterButton::None;
    };

    class ModalCharacter : public frame::gui::GuiWindowInterface {
    public:
        ModalCharacter(
            const std::string& name,
            ModalCharacterParams& params);
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;

    private:
        char name_buffer_[64] = { 0 };
        std::string name_;
        ModalCharacterParams& params_;
        bool end_ = false;
        int selected_color_ = 0;
    };

} // End namespace darwin::modal.