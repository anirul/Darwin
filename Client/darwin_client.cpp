#include "darwin_client.h"

namespace darwin {

    DarwinClient::DarwinClient(const std::string& name, std::shared_ptr<grpc::Channel> channel)
        : stub_(proto::DarwinService::NewStub(channel)), name_(name) {}

    void DarwinClient::Push(const proto::Physic& physic) {
        proto::PushRequest request;
        request.set_name(name_);
        *request.mutable_physic() = physic;

        proto::PushResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->Push(&context, request, &response);
        if (status.ok()) {
            std::cout << "Push successful!" << std::endl;
        }
        else {
            std::cout << "Push failed: " << status.error_code() << ": " << status.error_message() << std::endl;
        }
    }

    void DarwinClient::Update(WorldClient& world_client) {
        proto::UpdateRequest request;
        request.set_name(name_);

        proto::UpdateResponse response;
        grpc::ClientContext context;

        // The response stream
        std::unique_ptr<grpc::ClientReader<proto::UpdateResponse>> reader(
            stub_->Update(&context, request));

        // Read the stream of responses
        while (reader->Read(&response)) {
            // Process each response
            std::cout << "Received update for time: " << response.time() << std::endl;

            world_client.SetElements({ response.elements().begin(), response.elements().end() });
            world_client.SetPlayers({ response.players().begin(), response.players().end() });
        }

        // Finish the stream
        grpc::Status status = reader->Finish();
        if (!status.ok()) {
            std::cout << "Update stream failed: " << status.error_message() << std::endl;
        }
    }

    bool DarwinClient::IsConnected() const {
        return stub_ != nullptr;
    }

} // namespace darwin.