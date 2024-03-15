#include "modal_stats.h"

#include <imgui.h>

#include "Common/world_simulator.h"

namespace darwin::modal {

    ModalStats::ModalStats(
        const std::string& name,
        ModalStatsParams& params)
        : name_(name), params_(params)
    {
    }

    bool ModalStats::DrawCallback()
    {
        ImGui::Columns(6);
        ImGui::Text("Name");
        ImGui::NextColumn();
        ImGui::Text("Color");
        ImGui::NextColumn();
        ImGui::Text("Mass");
        ImGui::NextColumn();
        ImGui::Text("Status");
        ImGui::NextColumn();
        ImGui::Text("Position");
        ImGui::NextColumn();
        ImGui::Text("Speed");
        ImGui::NextColumn();
        for (const auto& character : characters_) {
            ImGui::Text("%s", character.name().c_str());
            ImGui::NextColumn();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(
                character.color().x(),
                character.color().y(),
                character.color().z(),
                1.0f));
            ImGui::Text("(*)");
            ImGui::PopStyleColor();
            ImGui::NextColumn();
            ImGui::Text("%.2f", character.physic().mass());
            ImGui::NextColumn();
            switch (character.status_enum()) {
            case proto::STATUS_DEAD:
                ImGui::Text("Dead");
                break;
            case proto::STATUS_JUMPING:
                ImGui::Text("Jumping");
                break;
            case proto::STATUS_LOADING:
                ImGui::Text("Loading");
                break;
            case proto::STATUS_ON_GROUND:
                ImGui::Text("On ground");
                break;
            case proto::STATUS_UNKNOWN:
                ImGui::Text("Unknown");
                break;
            default:
                // Should not happen.
                ImGui::Text("?");  
                break;
            }
            ImGui::NextColumn();
            ImGui::Text(
                "(%.2f, %.2f, %.2f)",
                character.physic().position().x(),
                character.physic().position().y(),
                character.physic().position().z());
            ImGui::NextColumn();
            ImGui::Text(
                "(%.2f, %.2f, %.2f)",
                character.physic().position_dt().x(),
                character.physic().position_dt().y(),
                character.physic().position_dt().z());
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
        if (params_.show_close_button) {
            if (ImGui::Button("Close")) {
                params_.button_result = ModalStatsButton::Close;
                end_ = true;
            }
        }
        return true;
    }

    bool ModalStats::End() const
    {
        return end_;
    }

    std::string ModalStats::GetName() const
    {
        return name_;
    }

    void ModalStats::SetName(const std::string& name)
    {
        name_ = name;
    }

} // namespace darwin::modal.