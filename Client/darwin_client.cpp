#include "darwin_client.h"

#include <glm/glm.hpp>
#include <grpc++/grpc++.h>

#include "async_client_call.h"
#include "Common/darwin_service.grpc.pb.h"
#include "Common/vector.h"
#include "Common/convert_math.h"

namespace darwin {

    DarwinClient::DarwinClient(const std::string& name)
        : name_(name) {
        if (name_ == "") {
            name_ = DEFAULT_SERVER;
        }
        auto channel = 
            grpc::CreateChannel(
                name_, 
                grpc::InsecureChannelCredentials());
        stub_ = proto::DarwinService::NewStub(channel);
        // Create a new thread to the update.
        update_future_ = std::async(std::launch::async, [this] { Update(); });
        // Create a new thread to the poll.
        poll_future_ = std::async(std::launch::async, [this] { 
                PollCompletionQueue(); 
            });
    }

    DarwinClient::~DarwinClient() {
        end_.store(true);
        cq_.Shutdown();
        update_future_.wait();
        poll_future_.wait();
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
            world_simulator_.SetPlayerParameter(response.player_parameter());
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
        const proto::Physic& physic,
        const std::string& potential_hit) 
    {
        report_movement_request_.set_name(name);
        report_movement_request_.mutable_physic()->CopyFrom(physic);
        report_movement_request_.set_potential_hit(potential_hit);
    }

    void DarwinClient::SendReportMovement() {
        auto promise = 
            std::make_shared<std::promise<proto::ReportMovementResponse>>();

        // This shared state can be used to pass more information to the
        // completion handler.
        auto* call = new AsyncClientCall;
        call->request = report_movement_request_;
        call->response = std::make_shared<proto::ReportMovementResponse>();
        call->promise = promise;

        // Prepare the asynchronous call
        call->rpc = 
            stub_->PrepareAsyncReportMovement(
                &call->context, 
                call->request, 
                &cq_);

        // Start the call
        call->rpc->StartCall();

        // Request to receive the response
        call->rpc->Finish(call->response.get(), &call->status, (void*)call);
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

            if (report_movement_request_.name() != "") {
                SendReportMovement();
                report_movement_request_.set_name("");
            }

            std::vector<proto::Character> characters;
            for (const auto& character : response.characters()) {
                characters.push_back(MergeCharacter(character));
            }

            static std::size_t element_size = 0;
            if (element_size != response.elements_size()) {
                logger_->warn(
                    "Update response elements size: {}", 
                    response.elements_size());
                element_size = response.elements_size();
            }

            // Update the elements and characters.
            world_simulator_.UpdateData(
                {
                    response.elements().begin(),
                    response.elements().end()
                },
                {
                    characters.begin(),
                    characters.end()
                },
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

    proto::Character DarwinClient::MergeCharacter(
        proto::Character new_character)
    {
        if (world_simulator_.GetCharactersSize() == 0) {
            return new_character;
        }
        if (new_character.name() == character_name_) {
            new_character = CorrectCharacter(
                new_character,
                world_simulator_.GetCharacterByName(character_name_));
        }
        return new_character;
    }

    proto::Character DarwinClient::CorrectCharacter(
        const proto::Character& server_character,
        const proto::Character& client_character) const
    {
        static auto planet_physic = world_simulator_.GetPlanet();
        proto::Character character = client_character;
        if (glm::any(glm::isnan(
            ProtoVector2Glm(client_character.physic().position())))) 
        {
            logger_->error(
                "Character [{}].position() is not a number.",
                character_name_);
            character.mutable_physic()->mutable_position()->CopyFrom(
                server_character.physic().position());
        }
        if (glm::any(glm::isnan(
            ProtoVector2Glm(client_character.physic().position_dt())))) 
        {
            logger_->error(
                "Character [{}].position_dt() is not a number.",
                character_name_);
            character.mutable_physic()->mutable_position_dt()->CopyFrom(
                Normalize(server_character.physic().position()));
        }
        if (glm::any(glm::isnan(ProtoVector2Glm(client_character.normal()))))
        {
            logger_->error(
                "Character [{}].normal() is not a number.",
                character_name_);
            character.mutable_normal()->CopyFrom(
                Normalize(server_character.normal()));
        }
        if (glm::any(glm::isnan(ProtoVector2Glm(client_character.g_force()))))
        {
            logger_->error(
                "Character [{}].g_force() is not a number.",
                character_name_);
            character.mutable_g_force()->CopyFrom(
                server_character.g_force());
        }
        character.mutable_physic()->set_mass(
            server_character.physic().mass());
        character.mutable_physic()->set_radius(
            server_character.physic().radius());
        return character;
    }

    void DarwinClient::PollCompletionQueue() {
        void* tag;
        bool ok;
        while (cq_.Next(&tag, &ok)) {
            auto* call = static_cast<AsyncClientCall*>(tag);
            if (!ok) {
                // Handle error. You might want to set an exception on the promise.
                logger_->warn("ReportMovement failed.");
                call->promise->set_exception(std::make_exception_ptr(std::runtime_error("RPC failed")));
            }
            else {
                // RPC succeeded, set the value on the promise
                call->promise->set_value(*(call->response));
            }
            // Clean up
            delete call;
        }
    }

} // namespace darwin.
