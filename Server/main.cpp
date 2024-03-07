#include <grpc++/grpc++.h>

#include <chrono>
#include <future>
#include <absl/flags/flag.h>
#include <absl/flags/parse.h>

#include "Server/darwin_service_impl.h"
#include "world_state_file.h"

ABSL_FLAG(
    std::string, 
    server_name, 
    "0.0.0.0:45323", 
    "The name of the server to connect to.");
ABSL_FLAG(
    std::string, 
    world_db, 
    "world_db.json", 
    "The name of the world database file.");

int main(int ac, char** av) try {
    absl::ParseCommandLine(ac, av);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(
        absl::GetFlag(FLAGS_server_name),
        grpc::InsecureServerCredentials());
    auto cq = builder.AddCompletionQueue();
    darwin::WorldState world_state;
    std::cout 
        << "loading world state from file: "<< absl::GetFlag(FLAGS_world_db) 
        << "\n";
    LoadWorldStateFromFile(world_state, absl::GetFlag(FLAGS_world_db));
    world_state.AddRandomElements(400);
    darwin::DarwinServiceImpl service{ cq.get(), world_state };

    std::cout << "starting world simulation\n";
    // Create a callback that will compute the next epoch.
    auto future = std::async(std::launch::async, [&service] {
        service.ComputeWorld();
    });

    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "listening on: " << absl::GetFlag(FLAGS_server_name) << "\n";

    // Uniquelly identify a request.
    void* tag;
    bool ok;
    while (true) {
        cq->Next(&tag, &ok);
        static_cast<darwin::DarwinServiceImpl*>(tag)->ListenForUpdates();
    }
    server->Shutdown();
    // Wait for the future to finish.
    future.wait();
    return 0;
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
