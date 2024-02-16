#pragma once

#include <functional>
#include <mutex>

#include "frame/gui/gui_window_interface.h"

namespace darwin::modal {

    struct ModalPingParams {
        std::int32_t ping_value = 45233;
        bool succeeded = false;
    };

    class ModalPing : public frame::gui::GuiWindowInterface {
    public:
        ModalPing(
            const std::string& name, 
            ModalPingParams& params, 
            std::function<bool()> callback);
        bool DrawCallback() override;
        bool End() const override;
        std::string GetName() const override;
        void SetName(const std::string& name) override;

    private:
        mutable std::thread thread_;
        std::mutex mutex_;
        std::string name_;
        ModalPingParams& params_;
        bool end_ = false;
    };

} // End namespace darwin::modal.