#include "modal_server.h"

#include <imgui.h>

namespace darwin::modal {

    bool ModalServer::DrawCallback() {
        static char server[64] = { '\0' };
        if (ImGui::InputText(
            "Server", 
            server, 
            64,
            ImGuiInputTextFlags_CharsNoBlank)) {
            params_.server_name = server;
        }
        if (ImGui::Button("Connect")) {
            params_.button_result = ModalServerButton::Connect;
            end_ = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            params_.button_result = ModalServerButton::Cancel;
            end_ = true;
        }
        return true;
    }

    bool ModalServer::End() const {
        return end_;
    }

    std::string ModalServer::GetName() const {
        return name_;
    }

    void ModalServer::SetName(const std::string& name) {
        name_ = name;
    }

} // namespace darwin::modal.

