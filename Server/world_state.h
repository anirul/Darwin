#pragma once

#include "Common/darwin_service.grpc.pb.h"
#include "Server/element_info.h"
#include "Server/player_info.h"

namespace darwin {

class WorldState {
 public:
  void AddPlayer(double time, const proto::Player& player);
  void AddElement(double time, const proto::Element& element);
  void Update(double time);
  const std::vector<proto::Player>& GetPlayers() const;
  const std::vector<proto::Element>& GetElement() const;

 private:
  std::map<std::string, PlayerInfo> player_infos_;
  std::map<std::string, ElementInfo> element_infos_;
  double last_updated_;
  std::vector<proto::Player> players_;
  std::vector<proto::Element> elements_;
};

}  // namespace darwin
