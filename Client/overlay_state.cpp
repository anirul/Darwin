#include "overlay_state.h"

#include "overlay_font.h"
#include "audio_system.h"

namespace darwin::overlay {

    OverlayState::OverlayState(
        const std::string name,
        const proto::ClientParameter& client_parameter,
        const proto::PageDescription& page_description,
        audio::AudioSystem& audio_system)
        : name_(name),
          page_description_(page_description),
          overlay_draw_(client_parameter, page_description),
          audio_system_(audio_system)
    {
        OverlayFont::GetInstance(client_parameter);
        start_time_ = std::chrono::system_clock::now();
    }

    bool OverlayState::DrawCallback() {
#ifdef _DEBUG
        overlay_draw_.Parameter("state_name", state_name_);
#endif
        auto duration = std::chrono::system_clock::now() - start_time_;
        overlay_draw_.Parameter(
            "timer_s", 
            std::chrono::duration<double>(duration).count());
        overlay_draw_.Parameter("music_name", audio_system_.GetMusicName());
        overlay_draw_.Parameter("authors_name", audio_system_.GetAuthorName());
        return overlay_draw_.Draw();
    }

    bool OverlayState::End() const {
        return false;
    }

    std::string OverlayState::GetName() const {
        return name_;
    }

    void OverlayState::SetName(const std::string& name) {
        name_ = name;
    }

} // namespace darwin::overlay.
