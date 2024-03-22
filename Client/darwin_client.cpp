#include "darwin_client.h"

#include <glm/glm.hpp>
#include <grpc++/grpc++.h>

#include "Common/stl_proto_wrapper.h"
#include "Common/darwin_service.grpc.pb.h"
#include "Common/client_parameter.pb.h"
#include "Common/vector.h"
#include "Common/convert_math.h"
#include "frame/file/file_system.h"

namespace darwin {

    DarwinClient::DarwinClient(
        const std::string& name,
        const proto::ClientParameter& client_parameter)
        : name_(name),
          client_parameter_(client_parameter)
    {
        if (name_.empty()) {
            name_ = client_parameter_.server_name();
        }
        auto channel = 
            grpc::CreateChannel(
                name_, 
                grpc::InsecureChannelCredentials());
        stub_ = proto::DarwinService::NewStub(channel);
        // Create a new thread to the update.
        update_future_ = std::async(std::launch::async, [this] { Update(); });
    }

    DarwinClient::~DarwinClient() {
        end_.store(true);
        update_future_.wait();
    }

    bool DarwinClient::CreateCharacter(
        const std::string& name, 
        const proto::Vector3& color) 
    {
        proto::CreateCharacterRequest request;
        request.set_name(name);
        request.mutable_color()->CopyFrom(color);
        Clear();

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

    void DarwinClient::RemovePreviousCharacter(const std::string& name) {
        std::scoped_lock l(mutex_);
        for (auto it = previous_characters_.begin();
            it != previous_characters_.end(); ++it)
        {
            if (it->first == name) {
                previous_characters_.erase(it);
                break;
            }
        }
    }

    void DarwinClient::Clear() {
        std::scoped_lock l(mutex_);
        report_request_.set_name(character_name_);
        world_simulator_.Clear();
        character_name_ = "";
    }

    void DarwinClient::ReportHit(const std::string& potential_hit) {
        std::scoped_lock l(mutex_);
        report_request_.set_potential_hit(potential_hit);
    }

    void DarwinClient::SendReportInGame() {
        SendReportInGameSync();
    }

    void DarwinClient::SendReportInGameSync() {
        std::scoped_lock l(mutex_);
        auto character = world_simulator_.GetCharacterByName(character_name_);
        report_request_.set_name(character_name_);
        report_request_.mutable_physic()->CopyFrom(character.physic());
        report_request_.set_status_enum(character.status_enum());
        proto::ReportInGameResponse response;
        grpc::ClientContext context;
        grpc::Status status = 
            stub_->ReportInGame(&context, report_request_, &response);
        if (!status.ok()) {
            logger_->warn("ReportInGame failed: {}.", status.error_message());
        }
        report_request_.set_potential_hit("");
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
            
            world_simulator_.SetUserName(character_name_);

            std::vector<proto::Character> characters;
            for (const auto& character : response.characters()) {
                auto name = character.name();
                auto status = character.status_enum();
                characters.push_back(MergeCharacter(character));
                previous_characters_.insert({ character.name(), character });
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

            // Send the report in game.
            SendReportInGame();

            // Check if the end is requested.
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
            world_simulator_.SetPlayerParameter(response.player_parameter());
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
        proto::Character new_character) const
    {
        // This does not exist in the world simulator.
        if (!previous_characters_.contains(new_character.name()) ||
            new_character.status_enum() == proto::STATUS_LOADING) {
            return new_character;
        }
        // This is the character from this client.
        if (new_character.name() == character_name_) {
            new_character = CorrectCharacter(
                new_character,
                world_simulator_.GetCharacterByName(character_name_));
            return new_character;
        }
        return new_character;
    }

    std::vector<proto::ColorParameter> 
        DarwinClient::GetColorParameters() const 
    {
        std::vector<proto::ColorParameter> color_parameters;
        const proto::PlayerParameter player_parameter = 
            world_simulator_.GetPlayerParameter();
        for (const auto& color_parameter : player_parameter.colors()) {
            color_parameters.push_back(color_parameter);
        }
        return color_parameters;
    }

    proto::Character DarwinClient::CorrectCharacter(
        const proto::Character& server_character,
        const proto::Character& client_character) const
    {
        const proto::Physic planet_physic = world_simulator_.GetPlanet();
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
                Normalize(server_character.physic().position_dt()));
        }
        if (glm::any(glm::isnan(ProtoVector2Glm(client_character.normal()))))
        {
            logger_->error(
                "Character [{}].normal() is not a number.",
                character_name_);
            character.mutable_normal()->CopyFrom(
                Normalize(server_character.normal()));
        }
        if (glm::any(glm::isnan(
            ProtoVector2Glm(client_character.g_force()))))
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

} // namespace darwin.
