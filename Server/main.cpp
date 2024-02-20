#include <grpc++/grpc++.h>

#include <chrono>
#include <thread>
#include <absl/flags/flag.h>
#include <absl/flags/parse.h>

#include "Server/compute_world.h"
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
    darwin::WorldState world_state;
    LoadWorldStateFromFile(world_state, absl::GetFlag(FLAGS_world_db));
    darwin::DarwinServiceImpl service;

    std::thread update_thread(
        darwin::ComputeWorld, 
        std::ref(service), 
        std::ref(world_state));

    builder.AddListeningPort(
        absl::GetFlag(FLAGS_server_name),
        grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    server->Wait();

    update_thread.join();
    return 0;
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
