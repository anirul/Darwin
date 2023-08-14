#include "darwin_service_impl.h"

namespace darwin {

grpc::Status DarwinServiceImpl::Update(
      grpc::ServerContext* context, 
      const proto::UpdateRequest* request,
      grpc::ServerWriter<proto::UpdateResponse>* writer) {
    while (true) {
        auto now_begin = std::chrono::system_clock::now();
      // Check if the client has cancelled the request
      if (context->IsCancelled()) {
        return grpc::Status::OK;
      }

      proto::UpdateResponse response;
      // TODO update a response.
      // Compute new physic.
      // Compute the move of players.
      // return the present state of the game.

      writer->Write(response);

      std::this_thread::sleep_until(now_begin +
          std::chrono::seconds(1));  // Sleep for 1 seconds
    }
    return grpc::Status::OK;
}

} // End namespace darwin.
