#include "darwin_client.h"

namespace darwin {

    DarwinClient::DarwinClient(const std::string& name)
        : name_(name) {
        if (name_ == "") {
            name_ = DEFAULT_SERVER;
        }
        else {
            name_ = name;
        }
        auto channel = 
            grpc::CreateChannel(
                name_, 
                grpc::InsecureChannelCredentials());
        stub_ = proto::DarwinService::NewStub(channel);
        // Create a new thread to the update.
        future_ = std::async(std::launch::async, [this] { 
                Update(world_client_);
            });
    }

    DarwinClient::~DarwinClient() {
        end_.store(true);
        future_.wait();
    }

    bool DarwinClient::CreateCharacter(
        const std::string& name, 
        const proto::Vector3& color) {
        proto::CreateCharacterRequest request;
        request.set_name(name);
        request.mutable_color()->CopyFrom(color);

        proto::CreateCharacterResponse response;
        grpc::ClientContext context;

        grpc::Status status = 
            stub_->CreateCharacter(&context, request, &response);
        if (status.ok()) {
            character_name_ = name;
            logger_->info("Create character: {}", name);
            return true;
        }
        else {
            logger_->warn(
                "Create character failed: {}", 
                status.error_message());
            return false;
        }
    }

    void DarwinClient::ReportMovement(
        const std::string& name,
        const proto::Physic& physic) 
    {
        proto::ReportMovementRequest request;
        request.set_name(name);
        request.mutable_physic()->CopyFrom(physic);

        proto::ReportMovementResponse response;
        grpc::ClientContext context;

        grpc::Status status = 
            stub_->ReportMovement(&context, request, &response);
        if (status.ok()) {
            logger_->info("Report movement physic: {}", physic.DebugString());
        }
        else {
            logger_->warn(
                "Report movement failed: {}", 
                status.error_message());
        }
    }

    void DarwinClient::Update(WorldClient& world_client) {
        proto::UpdateRequest request;
        request.set_name(name_);

        proto::UpdateResponse response;
        grpc::ClientContext context;

        // The response stream.
        std::unique_ptr<grpc::ClientReader<proto::UpdateResponse>> reader(
            stub_->Update(&context, request));

        // Read the stream of responses.
        while (reader->Read(&response)) {

            auto character_size = world_client.GetCharacters().size();
            auto element_size = world_client.GetElements().size();

            // Update the elements and characters.
            world_client.SetElements(
                { response.elements().begin(), 
                  response.elements().end() });

            world_client.SetCharacters(
                { response.characters().begin(), 
                  response.characters().end() });

            // Update the time.
            server_time_.store(response.time());

            if (character_size != world_client.GetCharacters().size()) {
                logger_->warn(
                    "Character changed size to: {}", 
                    world_client.GetCharacters().size());
            }
            if (element_size != world_client.GetElements().size()) {
                logger_->warn(
                    "Element changed size to: {}", 
                    world_client.GetElements().size());
            }

            if (end_.load()) {
                logger_->warn("Force exiting...");
                return;
            }
        }

        // Ensure you are at the end.
        end_.store(true);

        // Finish the stream
        grpc::Status status = reader->Finish();
        if (!status.ok()) {
            frame::Logger::GetInstance()->warn(
                "Update stream failed: {}", 
                status.error_message());
        }
    }

    std::int32_t DarwinClient::Ping(std::int32_t val) {
        proto::PingRequest request;
        request.set_value(val);

        proto::PingResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->Ping(&context, request, &response);
        if (status.ok()) {
            logger_->info(
                "Ping response server time: {}", 
                response.value(), 
                response.time());
            server_time_ = response.time();
            return response.value();
        }
        else {
            logger_->warn("Ping failed: {}", status.error_message());
            return 0;
        }
    }

    bool DarwinClient::IsConnected() const {
        return !end_.load();
    }

} // namespace darwin.