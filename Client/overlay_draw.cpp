#include "overlay_draw.h"

#include <sstream>
#include <iomanip>
#include <imgui.h>
#include <muParser.h>

#include "Common/vector.h"
#include "overlay_font.h"
#include "frame/logger.h"

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

    std::string OverlayDraw::ReplaceIndex(const std::string& text) const
    {
        if (index_int_ == -1) {
            return text;
        }
        auto pos = text.find(index_);
        if (pos != std::string::npos) {
            std::string replaced_text = text;
            replaced_text.replace(
                pos, 
                index_.size(), 
                std::to_string(index_int_));
            return replaced_text;
        }
        return text;
    }

    std::string OverlayDraw::ReplaceText(
        const std::string& text) const
    {
        std::string replaced_text = text;
        // Memoization.
        if (string_parameters_.contains(replaced_text)) {
            return string_parameters_.at(replaced_text);
        }
        // Used to replace string into the string.
        for (const auto& [name, value] : string_parameters_) {
            auto pos = replaced_text.find(name);
            if (pos != std::string::npos) {
                replaced_text.replace(pos, name.size(), value);
            }
        }
        // Used to replace double into string.
        for (const auto& [name, value] : double_parameters_) {
            auto pos = replaced_text.find(name);
            if (pos != std::string::npos) {
                std::ostringstream stream;
                stream << std::fixed << std::setprecision(2) << value;
                std::string value_as_string = stream.str();
                replaced_text.replace(pos, name.size(), value_as_string);
            }
        }
        string_parameters_[text] = replaced_text;
        return replaced_text;
    }

    double OverlayDraw::ReplaceDouble(
        const std::string& text) const
    {
        // Do the replace text before the computation to avoid trouble with
        // memoization.
        std::string replaced_index = ReplaceIndex(text);
        // Memoization.
        if (double_parameters_.contains(replaced_index)) {
            return double_parameters_.at(replaced_index);
        }
        try {
            mu::Parser parser;
            auto replaced_text = ReplaceText(replaced_index);
            parser.SetExpr(replaced_text);
            double answer = parser.Eval();
            double_parameters_[replaced_text] = answer;
            return answer;
        }
        catch (mu::Parser::exception_type& e) {
            throw std::runtime_error(e.GetMsg());
        }
    }

    ImVec2 OverlayDraw::ReplaceStart(
        const proto::PageElementLine& line,
        const proto::Vector2& alignment) const
    {
        if (line.has_start()) {
            return ImVec2(
                line.start().x() * size_.x() - alignment.x(),
                line.start().y() * size_.y() - alignment.y());
        }
        if (line.has_start_string()) {
            return ImVec2(
                ReplaceDouble(line.start_string().x()) * 
                size_.x() - alignment.x(),
                ReplaceDouble(line.start_string().y()) * 
                size_.y() - alignment.y());
        }
        throw std::runtime_error("No start found.");
    }

    ImVec2 OverlayDraw::ReplaceEnd(
        const proto::PageElementLine& line,
        const proto::Vector2& alignment) const
    {
        if (line.has_end()) {
            return ImVec2(
                line.end().x() * size_.x() - alignment.x(),
                line.end().y() * size_.y() - alignment.y());
        }
        if (line.has_end_string()) {
            return ImVec2(
                ReplaceDouble(
                    line.end_string().x()) * size_.x() - alignment.x(),
                ReplaceDouble(
                    line.end_string().y()) * size_.y() - alignment.y());
        }
        throw std::runtime_error("No end found.");
    }

    void OverlayDraw::DrawText(
        ImDrawList* draw_list,
        const proto::PageElementText& text)
    {
        // Push the font.
        OverlayFont::GetInstance(
            client_parameter_).PushFont(text.text_size_enum());
        // Replace the text.
        auto text_str = ReplaceIndex(text.text());
        text_str = ReplaceText(text_str);
        // Compute alignment.
        auto text_size = ImGui::CalcTextSize(text_str.c_str());
        auto alignment =
            GetAlignment(
                CreateVector2(text_size.x, text_size.y),
                text.alignment_enum());
        auto position = ReplacePosition(text, alignment);
        for (const auto& decoration : text.decorations()) {
            auto offset = ReplaceOffset(decoration);
            ImColor color = ReplaceColor(decoration);
            switch (decoration.decoration_enum()) {
                case proto::DECORATION_SHADOW :
                // Draw the shadow.
                draw_list->AddText(
                    ImVec2(position.x + offset.x, position.y + offset.y),
                    color,
                    text_str.c_str());
                break;
                case proto::DECORATION_OUTLINE :
                    // Draw the outline.
                    draw_list->AddRectFilled(
                        ImVec2(position.x - offset.x, position.y - offset.y),
                        ImVec2(
                            position.x + offset.x + text_size.x, 
                            position.y + offset.y + text_size.y),
                    color);
                    break;
                case proto::DECORATION_NONE:
                default:
                    throw std::runtime_error("Unknown decoration.");
            }
        }
        // Draw the text.
        draw_list->AddText(
            position,
            ReplaceColor(text),
            text_str.c_str());
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
            ReplacePosition(image, alignment),
            ReplacePositionSize(image, alignment));
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
            ReplaceStart(line, alignment),
            ReplaceEnd(line, alignment),
            ReplaceColor(line),
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
            ReplacePosition(rect_filled, alignment),
            ReplacePositionSize(rect_filled, alignment),
            ReplaceColor(rect_filled));
    }

    void OverlayDraw::DrawList(
        ImDrawList* draw_list,
        const proto::PageElementList& list)
    {
        index_ = list.element_index();
        int max_loop = static_cast<int>(ReplaceDouble(list.element_count()));
        for (int i = 0; i < max_loop; ++i) {
            index_int_ = i;
            // Draw the list.
            for (const auto& element : list.page_elements()) {
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
                case proto::PageElement::kList: 
                    [[fallthrough]];
                default:
                    throw std::runtime_error("Unknown page element.");
                }
            }
        }
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
            case proto::PageElement::kList: {
                DrawList(draw_list, element.list());
                break;
            }
            default:
                throw std::runtime_error("Unknown page element.");
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
