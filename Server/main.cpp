#include <grpc++/grpc++.h>

#include <chrono>
#include <thread>

#include "Server/darwin_service_impl.h"

int main() {
  grpc::ServerBuilder builder;
  darwin::DarwinServiceImpl service;

  builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  server->Wait();
  return 0;
}
