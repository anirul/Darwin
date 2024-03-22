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

    proto::Vector2 OverlayDraw::GetAlignment(
        const proto::Vector2& size, 
        proto::AlignmentEnum alignement_enum) const
    {
        proto::Vector2 alignement;
        switch (alignement_enum) {
        case proto::ALIGNMENT_TOP_LEFT:
            alignement = CreateVector2(0.0, 0.0);
            break;
        case proto::ALIGNMENT_TOP:
            alignement = CreateVector2(size.x() / 2.0, 0.0);
            break;
        case proto::ALIGNMENT_TOP_RIGHT:
            alignement = CreateVector2(size.x(), 0.0);
            break;
        case proto::ALIGNMENT_CENTER_LEFT:
            alignement = CreateVector2(0.0, size.y() / 2.0);
            break;
        case proto::ALIGNMENT_CENTER:
            alignement = CreateVector2(size.x() / 2.0, size.y() / 2.0);
            break;
        case proto::ALIGNMENT_CENTER_RIGHT:
            alignement = CreateVector2(size.x(), size.y() / 2.0);
            break;
        case proto::ALIGNMENT_BOTTOM_LEFT:
            alignement = CreateVector2(0.0, size.y());
            break;
        case proto::ALIGNMENT_BOTTOM:
            alignement = CreateVector2(size.x() / 2.0, size.y());
            break;
        case proto::ALIGNMENT_BOTTOM_RIGHT:    
            alignement = CreateVector2(size.x(), size.y());
            break;
        }
        return alignement;
    }

    void OverlayDraw::DrawText(
        ImDrawList* draw_list,
        const proto::PageElementText& text)
    {
        // Push the font.
        OverlayFont::GetInstance(
            client_parameter_).PushFont(text.text_size_enum());
        // Compute alignment.
        auto text_size = ImGui::CalcTextSize(text.text().c_str());
        auto alignment =
            GetAlignment(
                CreateVector2(text_size.x, text_size.y),
                text.alignment_enum());
        // Draw the text.
        draw_list->AddText(
            ImVec2(
                text.position().x() * size_.x() - alignment.x(),
                text.position().y() * size_.y() - alignment.y()),
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
        const proto::PageElementImage& image)
    {
        if (pointer_parameters_.contains(image.image()) == false) {
            return;
        }
        // Compute alignment.
        auto alignment =
            GetAlignment(
                CreateVector2(image.size().x(), image.size().y()),
                image.alignment_enum());
        // Draw the image.
        draw_list->AddImage(
            reinterpret_cast<ImTextureID>(pointer_parameters_[image.image()]),
            ImVec2(
                image.position().x() * size_.x() - alignment.x(),
                image.position().y() * size_.y() - alignment.y()),
            ImVec2(
                (image.position().x() + image.size().x()) * size_.x() - 
                    alignment.x(),
                (image.position().y() + image.size().y()) * size_.y() - 
                    alignment.y()));
    }

    void OverlayDraw::DrawLine(
        ImDrawList* draw_list,
        const proto::PageElementLine& line)
    {
        auto alignment =
            GetAlignment(
                CreateVector2(
                    line.end().x() - line.start().x(), 
                    line.end().y() - line.end().y()),
                line.alignment_enum());
        // Draw the line.
        draw_list->AddLine(
            ImVec2(
                line.start().x() * size_.x() - alignment.x(),
                line.start().y() * size_.y() - alignment.y()),
            ImVec2(
                line.end().x() * size_.x() - alignment.x(),
                line.end().y() * size_.y() - alignment.y()),
            ImColor(
                static_cast<float>(line.color().x()),
                static_cast<float>(line.color().y()),
                static_cast<float>(line.color().z()),
                static_cast<float>(line.color().w())),
            static_cast<float>(line.thickness()));
    }

    void OverlayDraw::DrawRectFilled(
        ImDrawList* draw_list,
        const proto::PageElementRectFilled& rect_filled) 
    {
        auto alignment =
            GetAlignment(
                CreateVector2(
                    rect_filled.size().x(),
                    rect_filled.size().y()),
                rect_filled.alignment_enum());
        // Draw the filled rectangle.
        draw_list->AddRectFilled(
            ImVec2(
                rect_filled.position().x() * size_.x() - alignment.x(),
                rect_filled.position().y() * size_.y() - alignment.y()),
            ImVec2(
                (rect_filled.position().x() + rect_filled.size().x()) * 
                    size_.x() - alignment.x(),
                (rect_filled.position().y() + rect_filled.size().y()) * 
                    size_.y() - alignment.y()),
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
