#pragma once

#include <string>
#include <filesystem>
#include <imgui.h>

#include "Common/client_parameter.pb.h"

namespace darwin::overlay {

    class OverlayFont {
    private:
        OverlayFont(
            const proto::ClientParameter& client_parameter);
    public:
        static OverlayFont& GetInstance(
            const proto::ClientParameter& client_parameter);
        void PushFont(proto::TextSizeEnum size);
        void PopFont();

    public:
        ImFont* GetFont(proto::TextSizeEnum size) {
            return fonts_[size];
        }

    private:
        std::map<proto::TextSizeEnum, ImFont*> fonts_;
        proto::ClientParameter client_parameter_;
    };

} // namespace darwin::overlay.
