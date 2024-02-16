#include "modal_ping.h"

#include <imgui.h>
#include <thread>

namespace darwin::modal {

    ModalPing::ModalPing(
        const std::string& name, 
        ModalPingParams& params, 
        std::function<bool()> callback) :
        name_(name), params_(params) {
        thread_ = std::thread([this, callback]() {
            bool temp = callback();
            std::lock_guard<std::mutex> lock(mutex_);
            params_.ping_value = (temp) ? params_.ping_value : 0;
            params_.succeeded = true;
            end_ = true;
        });
        thread_.detach();
    }

    bool ModalPing::DrawCallback() {
        ImGui::Text("Pinging...");
        if (ImGui::Button("Cancel")) {
            std::lock_guard<std::mutex> lock(mutex_);
            params_.ping_value = 0;
            params_.succeeded = true;
            end_ = true;
        }
        return true;
    }

    bool ModalPing::End() const {
        // thread_.join();
        return end_;
    }

    std::string ModalPing::GetName() const {
        return name_;
    }

    void ModalPing::SetName(const std::string& name) {
        name_ = name;
    }

} // namespace darwin::modal.
