#include "modal_character.h"

#include <imgui.h>

namespace darwin::modal {

    ModalCharacter::ModalCharacter(
        const std::string& name,
        ModalCharacterParams& params)
        : name_(name)
        , params_(params) {
        params_.color.set_x(1.0f);
        params_.color.set_y(0.0f);
        params_.color.set_z(0.0f);
    }

    bool ModalCharacter::DrawCallback() {
        char name[64] = { '\0' };
        if (ImGui::InputText(
            "Name",
            name,
            64,
            ImGuiInputTextFlags_CharsNoBlank)) {
            params_.name = name;
        }
        const ImVec4 colors[] = {
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
            ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
            ImVec4(0.0f, 0.0f, 1.0f, 1.0f),
            ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
            ImVec4(1.0f, 0.0f, 1.0f, 1.0f),
            ImVec4(0.0f, 1.0f, 1.0f, 1.0f),
        };
        const std::string color_names[] = {
            "Red",
            "Green",
            "Blue",
            "Yellow",
            "Magenta",
            "Cyan",
        };
        const size_t color_size = 6;
        if (ImGui::BeginCombo(
            "Colors", 
            color_names[selected_color_].c_str(), 
            0)) {
            for (int i = 0; i < color_size; i++) {
                const bool is_selected = (i == selected_color_);
                if (ImGui::Selectable(color_names[i].c_str(), is_selected)) {
                    selected_color_ = i;
                    params_.color.set_x(colors[i].x);
                    params_.color.set_y(colors[i].y);
                    params_.color.set_z(colors[i].z);
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        if (ImGui::Button("Create")) {
            if (params_.name.empty()) return true;
            params_.button_result = ModalCharacterButton::Create;
            end_ = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            params_.button_result = ModalCharacterButton::Cancel;
            end_ = true;
        }
        return true;
    }

    bool ModalCharacter::End() const {
        return end_;
    }

    std::string ModalCharacter::GetName() const {
        return name_;
    }

    void ModalCharacter::SetName(const std::string& name) {
        name_ = name;
    }

} // namespace darwin::modal.
