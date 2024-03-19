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
ABSL_FLAG(
    std::uint32_t,
    upgrade_count,
    400,
    "The maximum number of upgrade elements in the world.");
ABSL_FLAG(
    double,
    loop_timer,
    0.1,
    "The time in seconds between each world update.");

int main(int ac, char** av) try {
    absl::ParseCommandLine(ac, av);

    // Set grpc log handler.
    gpr_set_log_verbosity(GPR_LOG_SEVERITY_ERROR);
    grpc_tracer_set_enabled("all", true);

    grpc::ServerBuilder builder;
    darwin::WorldState world_state;
    std::cout 
        << "loading world state from file: "<< absl::GetFlag(FLAGS_world_db)
        << "\n";
    LoadWorldStateFromFile(world_state, absl::GetFlag(FLAGS_world_db));
    world_state.SetUpgradeElement(absl::GetFlag(FLAGS_upgrade_count));
    darwin::DarwinServiceImpl service{ world_state };

    double loop_timer = absl::GetFlag(FLAGS_loop_timer);
    std::cout << std::format(
        "starting world simulation with loop timer: {}\n", 
        loop_timer);
    // Create a callback that will compute the next epoch.
    auto future = std::async(std::launch::async, [&service, loop_timer] {
        service.ComputeWorld(loop_timer);
    });

    std::cout << "listening on: " << absl::GetFlag(FLAGS_server_name) << "\n";
    builder.AddListeningPort(
        absl::GetFlag(FLAGS_server_name),
        grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    server->Wait();

    // Wait for the future to finish.
    future.wait();
    return 0;
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
