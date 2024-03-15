#pragma once

#include "Common/darwin_service.grpc.pb.h"
#include "Common/proto_helper.h"
#include "Server/element_info.h"
#include "Server/character_info.h"

namespace darwin {

    class WorldState {
    public:
        bool CreateCharacter(
            const std::string& peer,
            const std::string& name, 
            const proto::Vector3& color);
        // This is there for testing purposes don't use in production.
        void AddCharacter(double time, const proto::Character& character);
        void AddRandomElements(std::uint32_t number);
        void RemoveCharacter(const std::string& name);
        std::string RemovePeer(const std::string& peer);
        bool IsCharacterOwnByPeer(
            const std::string& name, 
            const std::string& peer) const;
        std::optional<proto::Character> GetCharacterOwnedByPeer(
            const std::string& peer,
            const std::string& character_name) const;
        bool HasCharacter(const std::string& name) const;
        void UpdateCharacter(
            double time, 
            const std::string& name,
            proto::StatusEnum status,
            const proto::Physic& physic);
        void AddElement(double time, const proto::Element& element);
        void SetPlayerParameter(const proto::PlayerParameter& parameter);
        void Update(double time);
        double GetLastUpdated() const;
        bool operator==(const WorldState& other) const;
        proto::Element GetPlanet() const;
        void SetCharacterHits(
            const std::map<proto::Character, std::string>& character_hits);
        void UpdatePing(const std::string& name);

    public:
        proto::PlayerParameter GetPlayerParameter() const {
            return player_parameter_;
        }
        const std::vector<proto::Character>& GetCharacters() const {
            return characters_;
        }
        const std::vector<proto::Element>& GetElements() const {
            return elements_;
        }

    private:
        std::string RemovePeerLocked(const std::string& peer);
        void CheckStillInUseCharactersLocked();
        void CheckGroundCharactersLocked();
        void CheckDeathCharactersLocked();
        void CheckVictoryCharactersLocked();
        proto::Element GetPlanetLocked() const;
        void FillVectorsLocked();
        void CheckIntersectPlayerLocked();
        struct FromTo {
            std::string name_from;
            std::string name_to;
            proto::Physic physic_from;
            proto::Physic physic_to;
            proto::Vector3 color_from;
            proto::Vector3 color_to;
        };
        void ChangeSourceEatUpgradeLocked(const FromTo& from_to);
        void ChangeSourceEatCharacterLocked(const FromTo& from_to);
        void LostSourceElementLocked(const FromTo& from_to);
        void LostSourceCharacterLocked(const FromTo& from_to);

    private:
        mutable std::mutex mutex_;
        std::map<std::string, CharacterInfo> character_infos_;
        std::map<std::string, double> last_seen_;
        std::map<std::string, ElementInfo> element_infos_;
        std::map<std::string, std::string> peer_characters_;
        double last_updated_ = 0.0;
        std::vector<proto::Character> characters_;
        std::vector<proto::Element> elements_;
        proto::PlayerParameter player_parameter_;
        std::map<proto::Character, std::string> character_hits_;
    };

}  // namespace darwin.
