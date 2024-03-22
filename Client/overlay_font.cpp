#include "overlay_font.h"

#include "frame/file/file_system.h"
#include "frame/opengl/gui/imgui_impl_opengl3.h"

namespace darwin::overlay {

    OverlayFont::OverlayFont(
        const proto::ClientParameter& client_parameter)
        : client_parameter_(client_parameter)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto font_file = frame::file::FindFile(client_parameter_.font_file());
        // Load the fonts.
        for (const auto& font_size : client_parameter_.font_sizes()) {
            fonts_[font_size.text_size_enum()] = 
                io.Fonts->AddFontFromFileTTF(
                    font_file.string().c_str(),
                    font_size.size());
        }
        ImGui_ImplOpenGL3_CreateFontsTexture();
    }

    OverlayFont& OverlayFont::GetInstance(
        const proto::ClientParameter& client_parameter)
    {
        static OverlayFont instance(client_parameter);
        return instance;
    }

    void OverlayFont::PushFont(proto::TextSizeEnum size)
    {
        ImGui::PushFont(fonts_[size]);
    }

    void OverlayFont::PopFont()
    {
        ImGui::PopFont();
    }

} // namespace darwin::overlay.
