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
                Update();
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

    void DarwinClient::Update() {
        proto::UpdateRequest request;
        request.set_name(name_);

        proto::UpdateResponse response;
        grpc::ClientContext context;

        // The response stream.
        std::unique_ptr<grpc::ClientReader<proto::UpdateResponse>> 
            reader(stub_->Update(&context, request));

        // Read the stream of responses.
        while (reader->Read(&response)) {

            std::vector<proto::Character> characters;
            for (int i = 0; i < response.characters_size(); ++i) {
                if ((response.characters(i).name() == character_name_) && 
                    (world_simulator_.GetCharacterByName(
                        character_name_).status_enum() !=
                        proto::STATUS_UNKNOWN))
                {
                    characters.push_back(
                        world_simulator_.GetCharacterByName(character_name_));
                }
                else {
                    characters.push_back(response.characters(i));
                }
            }

            // Update the elements and characters.
            world_simulator_.UpdateData(
                { response.elements().begin(), 
                  response.elements().end() },
                { characters.begin(), characters.end() },
                response.time());
            
            // Update the time.
            server_time_.store(response.time());

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