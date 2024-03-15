#include "modal_death.h"

#include <imgui.h>

namespace darwin::modal {

    ModalDeath::ModalDeath(
        const std::string& name, 
        ModalDeathParams& params) :
        name_(name), params_(params) 
    {
    }

    bool ModalDeath::DrawCallback() {
        ImGui::TextUnformatted("You are dead.");
        if (ImGui::Button("Respawn")) {
            params_.button_result = ModalDeathButton::Respawn;
            end_ = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Exit")) {
            params_.button_result = ModalDeathButton::Exit;
            end_ = true;
        }
        return true;
    }

    bool ModalDeath::End() const {
        return end_;
    }

    std::string ModalDeath::GetName() const {
        return name_;
    }

    void ModalDeath::SetName(const std::string& name) {
        name_ = name;
    }

} // End namespace darwin::modal.