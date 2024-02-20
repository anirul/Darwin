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
                std::chrono::milliseconds(500));  // Just an example
        }
#ifdef _DEBUG
        std::cout << 
            std::format(
                "[{}] Removed a writer {}\n",
                context->peer(),
                request->name());
#endif // _DEBUG
        {
            std::lock_guard<std::mutex> lock(writers_mutex_);
            writers_.remove(writer);
        }
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
        {
#endif // _DEBUG
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
        }
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
        throw std::runtime_error("Not implemented");
        return grpc::Status::OK;
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

} // End namespace darwin.
