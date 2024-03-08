#include "modal_victory.h"

#include <imgui.h>

namespace darwin::modal {

    ModalVictory::ModalVictory(
        const std::string& name,
        ModalVictoryParams& params) :
        name_(name), params_(params)
    {
    }

    bool ModalVictory::DrawCallback() {
        ImGui::TextUnformatted("You are victorious.");
        if (ImGui::Button("Respawn")) {
            params_.button_result = ModalVictoryButton::Respawn;
            end_ = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Exit")) {
            params_.button_result = ModalVictoryButton::Exit;
            end_ = true;
        }
        return true;
    }

    bool ModalVictory::End() const {
        return end_;
    }

    std::string ModalVictory::GetName() const {
        return name_;
    }

    void ModalVictory::SetName(const std::string& name) {
        name_ = name;
    }

} // End namespace darwin::modal.
