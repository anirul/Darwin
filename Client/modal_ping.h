#pragma once

#include <functional>
#include <mutex>
#include <future>

#include "frame/gui/gui_window_interface.h"

namespace darwin::modal {

    enum class ModalPingButton {
        None,
        Pong,
        Cancel,
    };

    struct ModalPingParams {
        std::int32_t ping_value = 45233;
        ModalPingButton button_result = ModalPingButton::None;
    };

    class ModalPing : public frame::gui::GuiWindowInterface {
    public:
        ModalPing(
            const std::string& name, 
            ModalPingParams& params, 
            std::function<ModalPingParams()> callback);
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;

    private:
        std::string name_;
        ModalPingParams& params_;
        bool end_ = false;
        std::future<ModalPingParams> future_;
    };

} // End namespace darwin::modal.