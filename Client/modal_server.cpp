#include "modal_server.h"

#include <imgui.h>

#include "darwin_client.h"

namespace darwin::modal {

    ModalServer::ModalServer(
        const std::string& name, 
        ModalServerParams& params) :
        name_(name), 
        params_(params) 
    {
        std::copy(DEFAULT_SERVER.begin(), DEFAULT_SERVER.end(), server_);
    }


    bool ModalServer::DrawCallback() {
        if (ImGui::InputText(
            "Server",
            server_,
            64,
            ImGuiInputTextFlags_CharsNoBlank))
        {
            params_.server_name = server_;
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

