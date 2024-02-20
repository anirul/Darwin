#include "Server/compute_world.h"

#include <chrono>
#include <optional>

namespace darwin {

    void ComputeWorld(
        darwin::DarwinServiceImpl& service, 
        WorldState& world_state)
    {
        while (true) {
            auto now = std::chrono::system_clock::now();
            double time =
                std::chrono::duration_cast<std::chrono::duration<double>>(
                    now.time_since_epoch())
                .count();
            // Update the players.
            {
                std::lock_guard<std::mutex> lock(
                    service.GetTimeCharacterMutex());
                for (const auto& time_player : service.GetTimeCharacters()) {
                    world_state.UpdateCharacter(
                        time_player.first, 
                        time_player.second.name(), 
                        time_player.second.physic());
                }
                service.ClearTimeCharacters();
            }
            // Update the elements in the world.
            world_state.Update(time);
            const auto& elements = world_state.GetElements();
            const auto& characters = world_state.GetCharacters();
            proto::UpdateResponse response;
            response.mutable_elements()->CopyFrom(
                { elements.begin(), elements.end() });
            response.mutable_characters()->CopyFrom(
                { characters.begin(), characters.end() });
            response.set_time(time);
            service.BroadcastUpdate(response);
            std::this_thread::sleep_until(
                now + std::chrono::milliseconds(100));
        }
    }

}  // namespace darwin.
