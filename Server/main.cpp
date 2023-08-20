#include <grpc++/grpc++.h>

#include <chrono>
#include <thread>

#include "Server/compute_world.h"
#include "Server/darwin_service_impl.h"

int main() {
  grpc::ServerBuilder builder;
  darwin::DarwinServiceImpl service;

  std::thread update_thread(darwin::ComputeWorld, std::ref(service));

  builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  server->Wait();
  
  update_thread.join();
  return 0;
}
