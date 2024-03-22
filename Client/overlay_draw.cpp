#include "overlay_draw.h"

#include <imgui.h>

#include "Common/vector.h"
#include "overlay_font.h"

namespace darwin::overlay {

    OverlayDraw::OverlayDraw(
        const proto::ClientParameter& client_parameter,
        const proto::PageDescription& page_description)
        : client_parameter_(client_parameter),
          page_description_(page_description)
    {
        OverlayFont::GetInstance(client_parameter_);
    }

    void OverlayDraw::Parameter(
        const std::string& name, 
        const std::string& value) 
    {
        string_parameters_[name] = value;
    }

    void OverlayDraw::Parameter(const std::string& name, double value) {
        double_parameters_[name] = value;
    }

    void OverlayDraw::Parameter(const std::string& name, void* pointer) {
        pointer_parameters_[name] = pointer;
    }

    void OverlayDraw::DrawText(
        ImDrawList* draw_list,
        const proto::PageElementText& text) {
        // Draw the text.
        OverlayFont::GetInstance(
            client_parameter_).PushFont(text.text_size_enum());
        draw_list->AddText(
            ImVec2(
                text.position().x() * size_.x(), 
                text.position().y() * size_.y()),
            ImColor(
                static_cast<float>(text.color().x()),
                static_cast<float>(text.color().y()),
                static_cast<float>(text.color().z()),
                static_cast<float>(text.color().w())),
            text.text().c_str());
        OverlayFont::GetInstance(client_parameter_).PopFont();
    }

    void OverlayDraw::DrawImage(
        ImDrawList* draw_list,
        const proto::PageElementImage& image) {
        if (pointer_parameters_.contains(image.image()) == false) {
            return;
        }
        // Draw the image.
        draw_list->AddImage(
            reinterpret_cast<ImTextureID>(pointer_parameters_[image.image()]),
            ImVec2(
                image.position().x() * size_.x(), 
                image.position().y() * size_.y()),
            ImVec2(
                (image.position().x() + image.size().x()) * size_.x(),
                (image.position().y() + image.size().y()) * size_.y()));
    }

    void OverlayDraw::DrawLine(
        ImDrawList* draw_list,
        const proto::PageElementLine& line) {
        // Draw the line.
        draw_list->AddLine(
            ImVec2(line.start().x() * size_.x(), line.start().y() * size_.y()),
            ImVec2(line.end().x() * size_.x(), line.end().y() * size_.y()),
            ImColor(
                static_cast<float>(line.color().x()),
                static_cast<float>(line.color().y()),
                static_cast<float>(line.color().z()),
                static_cast<float>(line.color().w())),
            static_cast<float>(line.thickness()));
    }

    void OverlayDraw::DrawRectFilled(
        ImDrawList* draw_list,
        const proto::PageElementRectFilled& rect_filled) {
        // Draw the filled rectangle.
        draw_list->AddRectFilled(
            ImVec2(
                rect_filled.position().x() * size_.x(),
                rect_filled.position().y() * size_.y()),
            ImVec2(
                (rect_filled.position().x() + rect_filled.size().x()) * 
                    size_.x(),
                (rect_filled.position().y() + rect_filled.size().y()) * 
                    size_.y()),
            ImColor(
                static_cast<float>(rect_filled.color().x()),
                static_cast<float>(rect_filled.color().y()),
                static_cast<float>(rect_filled.color().z()),
                static_cast<float>(rect_filled.color().w())));
    }

    bool OverlayDraw::Draw() {
        // Draw the page.
        ImDrawList* draw_list = ImGui::GetForegroundDrawList();
        size_ = darwin::CreateVector2(
            ImGui::GetMainViewport()->Size.x, 
            ImGui::GetMainViewport()->Size.y);
        draw_list->PushClipRect(
            ImVec2(0, 0),
            ImVec2(ImGui::GetMainViewport()->Size));
        for (const auto& element : page_description_.page_elements()) {
            switch (element.PageElementOneof_case()) {
            case proto::PageElement::kText: {
                DrawText(draw_list, element.text());
                break;
            }
            case proto::PageElement::kRectFilled: {
                DrawRectFilled(draw_list, element.rect_filled());
                break;
            }
            case proto::PageElement::kLine: {
                DrawLine(draw_list, element.line());
                break;
            }
            case proto::PageElement::kImage: {
                DrawImage(draw_list, element.image());
                break;
            }
            }
        }
        draw_list->PopClipRect();
        // Reset the parameters.
        string_parameters_.clear();
        double_parameters_.clear();
        pointer_parameters_.clear();
        return true;
    }

} // namespace darwin::overlay.
