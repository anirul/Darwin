#include "darwin_service_impl.h"

#include <chrono>

namespace darwin {

    grpc::Status DarwinServiceImpl::Update(
        grpc::ServerContext* context,
        const proto::UpdateRequest* request,
        grpc::ServerWriter<proto::UpdateResponse>* writer)
    {
        // TODO(anirul): Check there is only one name.
#ifdef _DEBUG
        std::cout << 
            std::format(
                "Added a writer {}:{}\n", 
                request->name(), 
                context->peer());
#endif
        {
            std::lock_guard<std::mutex> lock(writers_mutex_);
            writers_.push_back(writer);
        }
        // This will block the connection, you can use a condition variable to
        // detect disconnect or a keep-alive mechanism.
        while (!context->IsCancelled()) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(500));  // Just an example
        }
#ifdef _DEBUG
        std::cout << 
            std::format(
                "Removed a writer {}:{}\n", 
                request->name(), 
                context->peer());
#endif
        {
            std::lock_guard<std::mutex> lock(writers_mutex_);
            writers_.remove(writer);
        }
        return grpc::Status::OK;
    }

    grpc::Status DarwinServiceImpl::Push(
        grpc::ServerContext* context,
        const proto::PushRequest* request,
        proto::PushResponse* response) 
    {
        std::cout << std::format("Got a push request from {}:{}\n", request->name(), context->peer());
        {
            std::lock_guard<std::mutex> lock(writers_mutex_);
            // Delete previous entry.
            proto::Player player;
            player.set_name(request->name());
            *player.mutable_physic() = request->physic();
            for (const auto& time_player : time_players_) {
                if (time_player.second.name() == request->name()) {
                    time_players_.erase(time_player.first);
                    break;
                }
            }
            auto now = std::chrono::system_clock::now();
            double time =
                std::chrono::duration_cast<std::chrono::duration<double>>(
                    now.time_since_epoch())
                .count();
            time_players_.insert({ time, player });
        }
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

    std::map<double, proto::Player>& DarwinServiceImpl::GetTimePlayers()
    {
        return time_players_;
    }

    void DarwinServiceImpl::ClearTimePlayers()
    {
        time_players_.clear();
    }

    std::mutex& DarwinServiceImpl::GetTimePLayersMutex()
    {
        return writers_mutex_;
    }

} // End namespace darwin.
