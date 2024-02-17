#include "modal_login.h"

#include <imgui.h>

namespace darwin::modal {

    bool ModalLogin::DrawCallback() {
        char username[64] = { '\0' };
        char password[64] = { '\0' };
        // Should probably add a callback for the username.
        ImGui::InputText(
            "Username", 
            username, 
            64,
            ImGuiInputTextFlags_CharsNoBlank);
        ImGui::InputText(
            "Password", 
            password, 
            64, 
            ImGuiInputTextFlags_Password);
        params_.username = username;
        params_.password = password;
        if (ImGui::Button("Login")) {
            params_.button_result = ModalLoginButton::Login;
            end_ = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            params_.button_result = ModalLoginButton::Cancel;
            end_ = true;
        }
        return true;
    }

    bool ModalLogin::End() const {
        return end_;
    }

    std::string ModalLogin::GetName() const {
        return name_;
    }

    void ModalLogin::SetName(const std::string& name) {
        name_ = name;
    }

} // namespace darwin::modal.
