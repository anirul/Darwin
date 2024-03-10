#include "modal_character.h"

#include <imgui.h>

namespace darwin::modal {

    ModalCharacter::ModalCharacter(
        const std::string& name,
        ModalCharacterParams& params,
        const std::vector<proto::ColorParameter>& colors) : 
        name_(name), params_(params), colors_(colors)
    {
        params_.color = colors_.begin()->color();
    }

    bool ModalCharacter::ColoredButton(
        const proto::ColorParameter& color_parameter) {
        ImVec4 color = ImVec4(
            color_parameter.color().x(),
            color_parameter.color().y(),
            color_parameter.color().z(),
            1.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImVec4 color_hovered = ImVec4(
            color_parameter.color().x() + 0.5f,
            color_parameter.color().y() + 0.5f,
            color_parameter.color().z() + 0.5f,
            1.0f);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
        ImVec4 color_active = ImVec4(
            color_parameter.color().x() - 0.5f,
            color_parameter.color().y() - 0.5f,
            color_parameter.color().z() - 0.5f,
            1.0f);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
        auto result = ImGui::Button(color_parameter.name().c_str());
        ImGui::PopStyleColor(3);
        return result;
    }

    bool ModalCharacter::DrawCallback() {
        if (ImGui::InputText(
            "Name",
            name_buffer_,
            64,
            ImGuiInputTextFlags_CharsNoBlank)) {
            params_.name = name_buffer_;
        }
        bool skip_first = true;
        for (const auto& color_parameter : colors_) {
            if (skip_first) {
                skip_first = false;
            }
            else {
                ImGui::SameLine();
            }
            if (ColoredButton(color_parameter)) {
                if (params_.name.empty()) return true;
                params_.button_result = ModalCharacterButton::Create;
                params_.color = color_parameter.color();
                end_ = true;
            }
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
