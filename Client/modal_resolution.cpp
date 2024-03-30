#include "modal_resolution.h"

#include <imgui.h>

namespace darwin::modal {

    ModalResolution::ModalResolution(
        const std::string& name,
        ModalResolutionParams& params) :
        name_(name),
        params_(params)
    {      
    }

    bool ModalResolution::DrawCallback() {
        
        if (ImGui::Button("Change")) {
            params_.button_result = ModalResolutionButton::Change;
        }
        ImGui::SameLine();
        if (ImGui::Button("Start!")) {
            params_.button_result = ModalResolutionButton::Start;
            end_ = true;
        }
    }

    bool ModalResolution::End() const {
        return end_;
    }

    std::string ModalResolution::GetName() const {
        return name_;
    }

    void ModalResolution::SetName(const std::string& name) {
        name_ = name;
    }

} // namespace darwin::modal.
