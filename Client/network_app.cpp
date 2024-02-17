#include "network_app.h"

namespace darwin {

    NetworkApp::NetworkApp() {}

    void NetworkApp::EnterWorld(const std::string& name) {
        if (name_ == "") {
            name_ = "localhost:45233";
        } 
        else {
            name_ = name;
        }
        if (darwin_client_) {
            darwin_client_.reset();
        }
        logger_->info("Enter world: {}", name_);
        // Create a connection to the server.
        darwin_client_ = std::make_unique<darwin::DarwinClient>(
            name_, 
            grpc::CreateChannel(
                name_, 
                grpc::InsecureChannelCredentials()));
        // Create a new thread to the update.
        std::thread update_thread(
            &DarwinClient::Update, 
            darwin_client_.get(), 
            std::ref(world_client_));
        update_thread.detach();
    }

    std::int32_t NetworkApp::Ping(std::int32_t val) {
        return darwin_client_->Ping(val);
    }

    std::string NetworkApp::GetName() const {
        return name_;
    }

    bool NetworkApp::IsConnected() const {
        return darwin_client_->IsConnected();
    }

    void NetworkApp::Run() {
    }

}  // namespace darwin.