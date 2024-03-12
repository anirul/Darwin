#include "darwin_client.h"

#include <glm/glm.hpp>
#include <grpc++/grpc++.h>

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

        // 5 seconds from now.
        std::chrono::system_clock::time_point deadline =
            std::chrono::system_clock::now() + std::chrono::seconds(10);
        // context.set_deadline(deadline);

        // The response stream.
        std::unique_ptr<grpc::ClientReader<proto::UpdateResponse>> 
            reader(stub_->Update(&context, request));

        double start_timer = 0.0;
        double delta_time = 0.1;
        bool first = true;

        // Read the stream of responses.
        while (reader->Read(&response)) {
            
            if (first) {
                first = false;
                start_timer = response.time();
            }
            else {
                delta_time = response.time() - start_timer;
                start_timer = response.time();
            }

            std::vector<proto::Character> characters;
            for (const auto& character : response.characters()) {
                characters.push_back(MergeCharacter(character, delta_time));
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
        proto::Character new_character,
        double delta_time)
    {
        // This does not exist in the world simulator.
        if (!world_simulator_.HasCharacter(new_character.name())) {
            return new_character;
        }
        // This is the character from this client.
        if (new_character.name() == character_name_) {
            new_character = CorrectCharacter(
                new_character,
                world_simulator_.GetCharacterByName(character_name_));
            return new_character;
        }
        // Not from this client interpolate.
        auto old_character = 
            world_simulator_.GetCharacterByName(new_character.name());
        return InterpolateCharacter(old_character, new_character, delta_time);
    }

    proto::Character DarwinClient::InterpolateCharacter(
        const proto::Character& old_character,
        const proto::Character& new_character,
        double delta_time) 
    {
        auto status = new_character.status_enum();
        if ((status != proto::STATUS_ON_GROUND) && 
            (status != proto::STATUS_JUMPING))
        {
            return new_character;
        }
        // Get the planet (will need the radius from it).
        const auto planet = world_simulator_.GetPlanet();
        // Update the position to the current position.
        proto::Vector3 updated_position = 
            Add(
                old_character.physic().position(),
                MultiplyVector3ByScalar(
                    old_character.physic().position_dt(),
                    delta_time));
        // Compute the future position from new_character.
        proto::Vector3 future_position =
            Add(
                new_character.physic().position(),
                MultiplyVector3ByScalar(
                    new_character.physic().position_dt(),
                    delta_time));
        // Create a new position_dt to update the position to the future.
        proto::Vector3 updated_position_dt =
            MultiplyVector3ByScalar(
                Subtract(
                    future_position,
                    updated_position),
                delta_time);
        proto::Character result = new_character;
        // On the ground.
        if (new_character.status_enum() == proto::STATUS_ON_GROUND) {
            // Get a normal to the position.
            auto normal = Normalize(updated_position);
            // Project the updated position_dt to the planet.
            updated_position_dt = ProjectOnPlane(updated_position_dt, normal);
            // Update the position to the planet.
            updated_position = 
                MultiplyVector3ByScalar(
                    normal, 
                    planet.radius() + new_character.physic().radius());
        }
        // Jumping.
        // Update the result.
        result.mutable_physic()->mutable_position()->CopyFrom(
            updated_position);
        result.mutable_physic()->mutable_position_dt()->CopyFrom(
            updated_position_dt);
        return result;
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

} // namespace darwin.
