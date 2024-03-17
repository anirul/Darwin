#include "darwin_service_impl.h"

#include <chrono>
#include <thread>
#include <glm/glm.hpp>

#include "Common/darwin_constant.h"
#include "Common/vector.h"
#include "Common/convert_math.h"
#include "world_state.h"

namespace darwin {

    grpc::Status DarwinServiceImpl::Update(
        grpc::ServerContext* context,
        const proto::UpdateRequest* request,
        grpc::ServerWriter<proto::UpdateResponse>* writer)
    {
#ifdef _DEBUG
        std::cout <<
            std::format(
                "[{}] Added a writer {}\n",
                context->peer(),
                request->name());
#endif
        {
            std::lock_guard<std::mutex> lock(writers_mutex_);
            writers_.push_back(writer);
        }
        // This will block the connection, you can use a condition variable to
        // detect disconnect or a keep-alive mechanism.
        while (!context->IsCancelled()) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100));  // Just an example
        }
#ifdef _DEBUG
        std::cout <<
            std::format(
                "[{}] Removed a writer {}\n", 
                context->peer(),
                request->name());
#endif // _DEBUG
        auto character_name = world_state_.RemovePeer(context->peer());
#ifdef _DEBUG
        if (!character_name.empty()) {
            std::cout <<
                std::format(
                    "[{}] Removed character {}\n",
                    context->peer(),
                    character_name);
        }
#endif // _DEBUG
        std::lock_guard<std::mutex> lock(writers_mutex_);
        writers_.remove(writer);
        return grpc::Status::OK;
    }

    grpc::Status DarwinServiceImpl::ReportInGame(
        grpc::ServerContext* context,
        const proto::ReportInGameRequest* request,
        proto::ReportInGameResponse* response)
    {
        std::lock_guard<std::mutex> lock(writers_mutex_);
        // Empty name check.
        if (request->name() == "") {
            return grpc::Status(
                grpc::StatusCode::INVALID_ARGUMENT,
                std::format("[{}]:{} Name is empty?",
                    context->peer(),
                    world_state_.GetLastUpdated()));
        }
        // Check if character is own by this peer.
        std::optional<proto::Character> maybe_character =
            world_state_.GetCharacterOwnedByPeer(
                context->peer(), 
                request->name());
        if (!maybe_character) {
            return grpc::Status(
                grpc::StatusCode::FAILED_PRECONDITION, 
                std::format("character {} don't exist?", request->name()));
        }
        // Update the physic.
        proto::Physic physic = UpdatePhysic(
            maybe_character.value().physic(),
            request->physic());
        maybe_character.value().mutable_physic()->CopyFrom(physic);
        auto status = request->status_enum();
        maybe_character.value().set_status_enum(status);
        maybe_character.value().mutable_physic()->set_mass(
            maybe_character.value().physic().mass() -
            world_state_.GetPlayerParameter().living_cost());
        auto now = std::chrono::system_clock::now();
        double time =
            std::chrono::duration_cast<std::chrono::duration<double>>(
                now.time_since_epoch())
            .count();
        time_characters_.insert({ time, maybe_character.value() });
        // Potential hit.
        if (!request->potential_hit().empty()) {
#ifdef _DEBUG
            std::cout << std::format(
                "[{}]:{} Got a potential hit from {}\n",
                context->peer(),
                world_state_.GetLastUpdated(),
                request->potential_hit());
#endif // _DEBUG
            if (!request->name().empty()) {
                character_hits_.insert(
                    { maybe_character.value(), request->potential_hit()});
            }
        }
        world_state_.UpdatePing(request->name());
        return grpc::Status::OK;
    }

    grpc::Status DarwinServiceImpl::CreateCharacter(
        grpc::ServerContext* context,
        const proto::CreateCharacterRequest* request,
        proto::CreateCharacterResponse* response)
    {
#ifdef _DEBUG
        std::cout <<
            std::format(
                "[{}] Got a create character request from {}\n",
                context->peer(),
                request->name());
#endif // _DEBUG
        const auto player_parameter = world_state_.GetPlayerParameter();
        bool found = false;
        for (const auto& color : player_parameter.colors()) {
            if (Dot(
                    Normalize(color.color()), 
                    Normalize(request->color())) <= 0.99) 
            {
                found = true;
                break;
            }
        }
        if (!found) {
            response->set_return_enum(proto::RETURN_REJECTED);
            return grpc::Status(
                grpc::StatusCode::INVALID_ARGUMENT,
                std::format(
                    "Color [{}] is not valid.", 
                    request->color().DebugString()));
        }
        if (world_state_.CreateCharacter(
            context->peer(),
            request->name(),
            request->color()))
        {
            response->set_return_enum(proto::RETURN_OK);
            return grpc::Status::OK;
        }
        else
        {
            response->set_return_enum(proto::RETURN_REJECTED);
            return grpc::Status(
                grpc::StatusCode::INVALID_ARGUMENT, 
                "Name [" + request->name() + "] is already in game.");
        }
    }

    grpc::Status DarwinServiceImpl::Ping(
        grpc::ServerContext* context,
        const proto::PingRequest* request,
        proto::PingResponse* response)
    {
#ifdef _DEBUG
        std::cout <<
            std::format(
                "[{}] Got a ping request from {}\n",
                context->peer(),
                request->value());
#endif // _DEBUG
        response->set_value(request->value());
        auto now = std::chrono::system_clock::now();
        double time =
            std::chrono::duration_cast<std::chrono::duration<double>>(
                now.time_since_epoch())
            .count();
        response->mutable_player_parameter()->CopyFrom(
            world_state_.GetPlayerParameter());
        response->set_time(time);
        return grpc::Status::OK;
    }

    void DarwinServiceImpl::BroadcastUpdate(
        const proto::UpdateResponse& response)
    {
        std::lock_guard<std::mutex> lock(writers_mutex_);
        for (auto writer : writers_) {
            writer->Write(response);
        }
    }

    std::map<double, proto::Character>& DarwinServiceImpl::GetTimeCharacters()
    {
        return time_characters_;
    }

    void DarwinServiceImpl::ClearTimeCharacters()
    {
        time_characters_.clear();
    }

    void DarwinServiceImpl::ComputeWorld(double loop_timer) {
        while (true) {
            auto now = std::chrono::system_clock::now();
            double time =
                std::chrono::duration_cast<std::chrono::duration<double>>(
                    now.time_since_epoch())
                .count();
            // Update the players.
            {
                std::lock_guard<std::mutex> lock(writers_mutex_);
                for (const auto& time_player : GetTimeCharacters()) {
                    world_state_.UpdateCharacter(
                        time_player.first,
                        time_player.second.name(),
                        time_player.second.status_enum(),
                        time_player.second.physic());
                }
                ClearTimeCharacters();
            }
            // Update the list of potential hits.
            world_state_.SetCharacterHits(character_hits_);
            character_hits_.clear();
            // Update the elements in the world.
            world_state_.Update(time);
            const auto& elements = world_state_.GetElements();
            const auto& characters = world_state_.GetCharacters();
            proto::UpdateResponse response;
            response.mutable_elements()->CopyFrom(
                { elements.begin(), elements.end() });
            response.mutable_characters()->CopyFrom(
                { characters.begin(), characters.end() });
            response.set_time(time);
            BroadcastUpdate(response);
            std::int64_t loop_time_milli = 
                static_cast<std::int64_t>(1000.0 * loop_timer);
            // Pring a warning if the computation is too slow.
            if ((now + std::chrono::milliseconds(loop_time_milli)) <
                std::chrono::system_clock::now())
            {
                std::cerr << 
                    std::format(
                        "ComputeWorld is too slow {} < {} !!!\n",
                        now + std::chrono::milliseconds(loop_time_milli),
                        std::chrono::system_clock::now());
            }
            // Wait for the next computation.
            std::this_thread::sleep_until(
                now + std::chrono::milliseconds(loop_time_milli));
        }
    }

    proto::Physic DarwinServiceImpl::UpdatePhysic(
        const proto::Physic& server_physic,
        const proto::Physic& client_physic) const
    {
        proto::Physic result = server_physic;
        if (glm::any(glm::isnan(
            ProtoVector2Glm(client_physic.position()))))
        {
            std::cerr << "Received a NaN position keep server value!\n";
            return result;
        }
        else if (Length(client_physic.position()) < 0.1f)
        {
            std::cerr << "Received a too small position keep server value!\n";
            return result;
        }
        else
        {
            result.mutable_position()->CopyFrom(
                client_physic.position());
        }
        if (glm::any(glm::isnan(
            ProtoVector2Glm(client_physic.position_dt()))))
        {
            std::cerr << "Received a NaN position_dt keep server value!\n";
        }
        else
        {
            result.mutable_position_dt()->CopyFrom(
                client_physic.position_dt());
        }
        return result;
    }

} // End namespace darwin.
