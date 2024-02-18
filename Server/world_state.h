#pragma once

#include "Common/darwin_service.grpc.pb.h"
#include "Server/element_info.h"
#include "Server/character_info.h"

namespace darwin {

    class WorldState {
    public:
        void AddCharacter(double time, const proto::Character& character);
        void UpdateCharacter(
            double time, 
            const std::string& name, 
            const proto::Physic& character);
        void AddElement(double time, const proto::Element& element);
        void Update(double time);
        const std::vector<proto::Character>& GetCharacters() const;
        const std::vector<proto::Element>& GetElements() const;
        double GetLastUpdated() const;
        bool operator==(const WorldState& other) const;

    private:
        void FillVectorsLocked();

    private:
        std::mutex mutex_info_;
        std::map<std::string, CharacterInfo> character_infos_;
        std::map<std::string, ElementInfo> element_infos_;
        double last_updated_ = 0.0;
        std::vector<proto::Character> characters_;
        std::vector<proto::Element> elements_;
    };

}  // namespace darwin
