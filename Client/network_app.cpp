#include "network_app.h"

namespace darwin {

    NetworkApp::NetworkApp() {}

    void NetworkApp::EnterWorld(const std::string& name) {
        name_ = name;
        if (darwin_client_) {
            darwin_client_.reset();
        }
        logger_->info("Enter world: {}", name);
        // Create a connection to the server.
        darwin_client_ = std::make_unique<darwin::DarwinClient>(
            name_, 
            grpc::CreateChannel(
                "localhost:50051", 
                grpc::InsecureChannelCredentials()));
        // Create a new thread to the update.
        std::thread update_thread(
            &DarwinClient::Update, 
            darwin_client_.get(), 
            std::ref(world_client_));
        update_thread.detach();
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