#pragma once

#include "Common/darwin_service.grpc.pb.h"
#include "Server/element_info.h"
#include "Server/player_info.h"

namespace darwin {

class PhysicEngine {
 public:
  PhysicEngine(std::map<std::string, ElementInfo>& element_infos,
               std::map<std::string, PlayerInfo>& player_infos)
      : element_infos_(element_infos), player_infos_(player_infos) {}
  void ComputeElementInfo(double delta);
  void ComputePlayerInfo(double delta);

 protected:
  std::map<std::string, ElementInfo>& element_infos_;
  std::map<std::string, PlayerInfo>& player_infos_;
};

}  // namespace darwin