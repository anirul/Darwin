#include "overlay_play.h"

#include "overlay_font.h"
#include "Common/vector.h"
#include "frame/logger.h"

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
        proto::Character player_character;
        // I need the normal to the camera to filter out characters that are
        // not in the fieald of view.
        for (const auto& character : characters_) {
            if (character.name() == character_name_) {
                player_character = character;
                overlay_draw_.Parameter(
                    "player_character.name",
                    character.name());
                overlay_draw_.Parameter(
                    "player_character.mass",
                    character.physic().mass());
                overlay_draw_.Parameter(
                    "player_character.color[0]",
                    character.color().x());
                overlay_draw_.Parameter(
                    "player_character.color[1]",
                    character.color().y());
                overlay_draw_.Parameter(
                    "player_character.color[2]",
                    character.color().z());
                break;
            }
        }
        std::map<double, proto::Character> characters_sorted;
        for (const auto& character : characters_) {
            if (Dot(
                Normalize(player_character.physic().position()), 
                Normalize(character.physic().position())) < 0.7) 
            {
                continue;
            }
            characters_sorted.insert({ 
                    Distance(
                        character.physic().position(), 
                        characters_[0].physic().position()), 
                    character
                });
        }
        int i = 0;
        for (const auto& [_, character] : characters_sorted) {
            if (i > 10) {
                overlay_draw_.Parameter("character[10].name", "and more...");
                overlay_draw_.Parameter("character[10].mass", 0.0);
                overlay_draw_.Parameter("character[10].color[0]", 0.0);
                overlay_draw_.Parameter("character[10].color[1]", 0.0);
                overlay_draw_.Parameter("character[10].color[2]", 0.0);
                ++i;
                break;
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
            ++i;
        }
        overlay_draw_.Parameter("character_count", i);
        if (!character_name_.empty() && !player_character.name().empty()) {
            overlay_draw_.Draw();
        }
        else {
            frame::Logger::GetInstance()->warn(
                "No character name or player_character has no name.");
        }
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
