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
        proto::Vector2 GetAlignment(
            const proto::Vector2& size,
            proto::AlignmentEnum alignment_enum) const;
        proto::PageElementText ReplaceText(
            const proto::PageElementText& text) const;
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

    private:
        proto::ClientParameter client_parameter_;
        proto::PageDescription page_description_;
        proto::Vector2 size_;
        std::map<std::string, std::string> string_parameters_;
        std::map<std::string, double> double_parameters_;
        std::map<std::string, void*> pointer_parameters_;
    };

} // namespace darwin::overlay.
