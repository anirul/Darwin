#include "overlay_play.h"

#include "overlay_font.h"
#include "Common/vector.h"

namespace darwin::overlay {

    OverlayPlay::OverlayPlay(
        const std::string& name,
        const proto::ClientParameter& client_parameter,
        const proto::PageDescription& page_description)
        : name_(name),
          page_description_(page_description),
          overlay_draw_(client_parameter, page_description)
    {
        OverlayFont::GetInstance(client_parameter);
    }

    bool OverlayPlay::DrawCallback() {
        overlay_draw_.Parameter("character_name", character_name_);
        overlay_draw_.Parameter(
            "character_count", 
            static_cast<int>(characters_.size()));
        proto::Character player_character;
        for (const auto& character : characters_) {
            if (character.name() == character_name_) {
                player_character = character;
                overlay_draw_.Parameter(
                    "player_character.name", 
                    player_character.name());
                overlay_draw_.Parameter(
                    "player_character.mass", 
                    player_character.physic().mass());
                overlay_draw_.Parameter(
                    "player_character.color[0]",
                    player_character.color().x());
                overlay_draw_.Parameter(
                    "player_character.color[1]",
                    player_character.color().y());
                overlay_draw_.Parameter(
                    "player_character.color[2]",
                    player_character.color().z());
                break;
            }
        }
        int i = 0;
        for (const auto& character : characters_) {
            if (character.name() == character_name_) {
                continue;
            }
            if (Dot(player_character.normal(), character.normal()) < 0.8) {
                continue;
            }
            overlay_draw_.Parameter(
                std::format("character[{}].name", i), 
                character.name());
            overlay_draw_.Parameter(
                std::format("character[{}].mass", i),
                character.physic().mass());
            overlay_draw_.Parameter(
                std::format("character[{}].color[0]", i),
                character.color().x());
            overlay_draw_.Parameter(
                std::format("character[{}].color[1]", i),
                character.color().y());
            overlay_draw_.Parameter(
                std::format("character[{}].color[2]", i),
                character.color().z());
            i++;
        }
        overlay_draw_.Draw();
        return true;
    }

    bool OverlayPlay::End() const {
        return false;
    }

    void OverlayPlay::SetName(const std::string& name) {
        name_ = name;
    }

    std::string OverlayPlay::GetName() const {
        return name_;
    }

} // namespace darwin::overlay.
