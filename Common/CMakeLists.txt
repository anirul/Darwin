# grpc-server Common part (mainly proto).
add_library(DarwinCommon
    STATIC
        service.proto
)

target_include_directories(DarwinCommon
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${Protobuf_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
)

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

protobuf_generate(TARGET DarwinCommon LANGUAGE cpp)
protobuf_generate(
    TARGET
        DarwinCommon
    LANGUAGE
        grpc
    GENERATE_EXTENSIONS
        .grpc.pb.h
        .grpc.pb.cc
    PLUGIN
        "protoc-gen-grpc=${grpc_cpp_plugin_location}"
)

target_link_libraries(DarwinCommon
    PRIVATE
        protobuf::libprotoc
        protobuf::libprotobuf
        gRPC::gpr
        gRPC::grpc
        gRPC::grpc++
)

set(HEADERS_TO_BE_COPIED
    service.pb.h
    service.grpc.pb.h
)

foreach(HEADERN ${HEADERS_TO_BE_COPIED})
    add_custom_command(
        TARGET DarwinCommon
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/Common/proto/${HEADERN}
            ${CMAKE_SOURCE_DIR}/Common/proto
    )
endforeach(HEADERN)

set_property(TARGET DarwinCommon PROPERTY FOLDER "DarwinCommon")
