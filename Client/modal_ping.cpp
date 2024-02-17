#include "modal_ping.h"

#include <imgui.h>

namespace darwin::modal {

    ModalPing::ModalPing(
        const std::string& name, 
        ModalPingParams& params, 
        std::function<ModalPingParams()> callback) :
        name_(name), params_(params) {
        future_ = std::async(std::launch::async, [callback]() {
            return callback();
        });
    }

    bool ModalPing::DrawCallback() {
        ImGui::Text("Pinging...");
        if (ImGui::Button("Cancel")) {
            params_.ping_value = 0;
            params_.button_result = ModalPingButton::Cancel;
            end_ = true;
        }
        auto status = future_.wait_for(std::chrono::milliseconds(0));
        if (status == std::future_status::ready) {
            auto result = future_.get();
            params_.ping_value = result.ping_value;
            params_.button_result = result.button_result;
            end_ = true;
        }
        return true;
    }

    bool ModalPing::End() const {
        return end_;
    }

    std::string ModalPing::GetName() const {
        return name_;
    }

    void ModalPing::SetName(const std::string& name) {
        name_ = name;
    }

} // namespace darwin::modal.
