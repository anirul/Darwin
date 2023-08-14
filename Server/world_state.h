#pragma once

#include "Common/darwin_service.grpc.pb.h"

namespace darwin {

class WorldState {
 public:
  void AddPlayer(double time, const proto::Player& player);
  void Update(double time);
  const std::vector<proto::Player>& GetPlayers() const;
  const std::vector<proto::Element>& GetElement() const;

 private:
  std::vector<proto::Player> players_;
  std::vector<proto::Element> elements_;
};

}  // namespace darwin