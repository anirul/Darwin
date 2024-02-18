#include "modal_disconnected.h"

#include <imgui.h>

namespace darwin::modal {

    ModalDisconnected::ModalDisconnected(
        const std::string& name,
        ModalDisconnectedParams& params) :
        name_(name), params_(params) {}

    bool ModalDisconnected::DrawCallback() {
        ImGui::Text("Disconnected from server");
        if (ImGui::Button("Reconnect")) {
            params_.button_result = ModalDisconnectedButton::Retry;
            end_ = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            params_.button_result = ModalDisconnectedButton::Cancel;
            end_ = true;
        }
        return true;
    }

    bool ModalDisconnected::End() const {
        return end_;
    }

    std::string ModalDisconnected::GetName() const {
        return name_;
    }

    void ModalDisconnected::SetName(const std::string& name) {
        name_ = name;
    }

} // namespace darwin::modal.
