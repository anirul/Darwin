#include "Server/compute_world.h"

#include <chrono>

namespace darwin {

void ComputeWorld(darwin::DarwinServiceImpl& service) {
  WorldState world_state;
  while (true) {
    auto now = std::chrono::system_clock::now();
    double time = std::chrono::duration_cast<std::chrono::duration<double>>(
                      now.time_since_epoch())
                      .count();
    world_state.Update(time);
    const auto& elements = world_state.GetElements();
    const auto& players = world_state.GetPlayers();
    proto::UpdateResponse response;
    response.mutable_elements()->CopyFrom({elements.begin(), elements.end()});
    response.mutable_players()->CopyFrom({players.begin(), players.end()});
    response.set_time(time);
    service.BroadcastUpdate(response);
    std::this_thread::sleep_until(now + std::chrono::milliseconds(500));
  }
}

}  // namespace darwin.
