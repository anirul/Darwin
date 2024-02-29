#include "darwin_service_impl.h"

#include <chrono>

#include "Common/vector.h"
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

    grpc::Status DarwinServiceImpl::ReportMovement(
        grpc::ServerContext* context,
        const proto::ReportMovementRequest* request,
        proto::ReportMovementResponse* response)
    {
#ifdef _DEBUG
        std::cout << std::format(
            "[{}] Got a push request from {}\n",
            context->peer(),
            request->name());
#endif // _DEBUG
        // Check if character is own by this peer.
        if (!world_state_.IsCharacterOwnByPeer(
            context->peer(),
            request->name())) {
            response->set_return_enum(proto::RETURN_REJECTED);
            return grpc::Status::CANCELLED;
        }
        std::lock_guard<std::mutex> lock(writers_mutex_);
        // Delete previous entry.
        proto::Character character;
        character.set_name(request->name());
        *character.mutable_physic() = request->physic();
        for (const auto& time_character : time_characters_) {
            if (time_character.second.name() == request->name()) {
                time_characters_.erase(time_character.first);
                break;
            }
        }
        auto now = std::chrono::system_clock::now();
        double time =
            std::chrono::duration_cast<std::chrono::duration<double>>(
                now.time_since_epoch())
            .count();
        time_characters_.insert({ time, character });
        response->set_return_enum(proto::RETURN_OK);
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
            return grpc::Status::CANCELLED;
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
        response->set_time(time);
        return grpc::Status::OK;
    }

    grpc::Status DarwinServiceImpl::DeathReport(
        grpc::ServerContext* context,
        const proto::DeathReportRequest* request,
        proto::DeathReportResponse* response)
    {
#ifdef _DEBUG
        std::cout <<
            std::format(
                "[{}] Got a death report from {}\n",
                context->peer(),
                request->name());
#endif // _DEBUG
        throw std::runtime_error("Not implemented");
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

    std::mutex& DarwinServiceImpl::GetTimeCharacterMutex()
    {
        return writers_mutex_;
    }

    void DarwinServiceImpl::ComputeWorld() {
        while (true) {
            auto now = std::chrono::system_clock::now();
            double time =
                std::chrono::duration_cast<std::chrono::duration<double>>(
                    now.time_since_epoch())
                .count();
            // Update the players.
            {
                std::lock_guard<std::mutex> lock(GetTimeCharacterMutex());
                for (const auto& time_player : GetTimeCharacters()) {
                    world_state_.UpdateCharacter(
                        time_player.first,
                        time_player.second.name(),
                        time_player.second.physic());
                }
                ClearTimeCharacters();
            }
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
            // Pring a warning if the computation is too slow.
            if ((now + std::chrono::milliseconds(INTERVAL)) <
                std::chrono::system_clock::now())
            {
                std::cerr << 
                    std::format(
                        "ComputeWorld is too slow {} < {} !!!\n",
                        now + std::chrono::milliseconds(INTERVAL),
                        std::chrono::system_clock::now());
            }
            std::this_thread::sleep_until(
                now + std::chrono::milliseconds(INTERVAL));
        }
    }

} // End namespace darwin.
