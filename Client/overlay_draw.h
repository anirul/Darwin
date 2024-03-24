#pragma once

#include <string>
#include <imgui.h>

#include "Common/client_parameter.pb.h"

namespace darwin::overlay {

    class OverlayDraw {
    public:
        // Constructor take the description of the page to draw.
        OverlayDraw(
            const proto::ClientParameter& client_parameter,
            const proto::PageDescription& page_description);
        // Parameter setters.
        void Parameter(const std::string& name, const std::string& value);
        void Parameter(const std::string& name, double value);
        void Parameter(const std::string& name, void* pointer);
        // Draw the page this will reset the parameters as well.
        bool Draw();

    protected:
        template<typename T>
        ImVec2 ReplacePosition(
            T position_container,
            const proto::Vector2& alignment) const
        {
            if (position_container.has_position()) {
                return ImVec2(
                    position_container.position().x() * size_.x() - 
                    alignment.x(),
                    position_container.position().y() * size_.y() - 
                    alignment.y());
            }
            if (position_container.has_position_string()) {
                return ImVec2(
                    ReplaceDouble(position_container.position_string().x()) *
                    size_.x() - alignment.x(),
                    ReplaceDouble(position_container.position_string().y()) *
                    size_.y() - alignment.y());
            }
            throw std::runtime_error("No position found.");
        }
        template<typename T>
        ImVec2 ReplaceSize(
            T size_container, 
            const proto::Vector2& alignment) const
        {
            if (size_container.has_size()) {
                return ImVec2(
                    size_container.size().x() * size_.x() - 
                    alignment.x(),
                    size_container.size().y() * size_.y() - 
                    alignment.y());
            }
            if (size_container.has_size_string()) {
                return ImVec2(
                    ReplaceDouble(size_container.size_string().x()) * 
                    size_.x() - alignment.x(),
                    ReplaceDouble(size_container.size_string().y()) * 
                    size_.y() - alignment.y());
            }
            throw std::runtime_error("No size found.");
        }
        template<typename T>
        ImVec2 ReplacePositionSize(
            T position_size_container, 
            const proto::Vector2& alignment) const 
        {
            auto pos = ReplacePosition(position_size_container, alignment);
            auto size = ReplaceSize(position_size_container, alignment);
            return ImVec2(pos.x + size.x, pos.y + size.y);
        }
        template<typename T>
        ImColor ReplaceColor(T color) const
        {
            if (color.has_color()) {
                return ImColor(
                    static_cast<float>(color.color().x()),
                    static_cast<float>(color.color().y()),
                    static_cast<float>(color.color().z()),
                    static_cast<float>(color.color().w()));
            }
            if (color.has_color_string()) {
                return ImColor(
                    static_cast<float>(
                        ReplaceDouble(color.color_string().red())),
                    static_cast<float>(
                        ReplaceDouble(color.color_string().green())),
                    static_cast<float>(
                        ReplaceDouble(color.color_string().blue())),
                    static_cast<float>(
                        ReplaceDouble(color.color_string().alpha())));
            }
            throw std::runtime_error("No color found.");
        }
        template<typename T>
        ImVec2 ReplaceOffset(T offset) const
        {
            if (offset.has_offset()) {
                return ImVec2(
                    offset.offset().x() * size_.x(),
                    offset.offset().y() * size_.y());
            }
            if (offset.has_offset_string()) {
                return ImVec2(
                    ReplaceDouble(offset.offset_string().x()) * size_.x(),
                    ReplaceDouble(offset.offset_string().y()) * size_.y());
            }
            throw std::runtime_error("No offset found.");
        }

    protected:
        ImVec2 ReplaceStart(
            const proto::PageElementLine& line,
            const proto::Vector2& alignment) const;
        ImVec2 ReplaceEnd(
            const proto::PageElementLine& line,
            const proto::Vector2& alignment) const;
        proto::Vector2 GetAlignment(
            const proto::Vector2& size,
            proto::AlignmentEnum alignment_enum) const;
        std::string ReplaceIndex(const std::string& text) const;
        std::string ReplaceText(const std::string& text) const;
        double ReplaceDouble(const std::string& text) const;
        void DrawText(
            ImDrawList* draw_list, 
            const proto::PageElementText& text);
        void DrawImage(
            ImDrawList* draw_list, 
            const proto::PageElementImage& image);
        void DrawLine(
            ImDrawList* draw_list, 
            const proto::PageElementLine& line);
        void DrawRectFilled(
            ImDrawList* draw_lit,
            const proto::PageElementRectFilled& rect_filled);
        void DrawList(
            ImDrawList* draw_list, 
            const proto::PageElementList& list);

    private:
        proto::ClientParameter client_parameter_;
        proto::PageDescription page_description_;
        proto::Vector2 size_;
        // These parameters are mutable to maintain the constness of the
        // calls (ReplaceText, ReplaceDouble, etc).
        mutable std::map<std::string, std::string> string_parameters_;
        mutable std::map<std::string, double> double_parameters_;
        // Used to index into the parameters.
        int index_int_ = -1;
        std::string index_ = "<index>";
        std::map<std::string, void*> pointer_parameters_;
    };

} // namespace darwin::overlay.
