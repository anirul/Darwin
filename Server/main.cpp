#include <grpc++/grpc++.h>

#include <chrono>
#include <thread>

#include "Server/compute_world.h"
#include "Server/darwin_service_impl.h"

int main(int ac, char** av) {
    grpc::ServerBuilder builder;
    darwin::DarwinServiceImpl service;
    darwin::WorldState world_state;

    std::thread update_thread(darwin::ComputeWorld, std::ref(service), std::ref(world_state));

    builder.AddListeningPort(
        "0.0.0.0:50051", 
        grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    server->Wait();

    update_thread.join();
    return 0;
}
