cmake_minimum_required(VERSION 3.20)

project(darwin 
    DESCRIPTION "Test of a grpc based server (and client)." 
    LANGUAGES CXX
)

# Standards is set to C++ 20.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Adding subfolder property.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# External packages
find_package(absl CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(gRPC)
find_package(muparser CONFIG REQUIRED)

# Add submodules build.
add_subdirectory(External/Frame)

# Sources.
add_subdirectory(Common)
add_subdirectory(Client)
add_subdirectory(Server)
add_subdirectory(Test)
