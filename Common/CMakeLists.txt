# grpc-server Common part (mainly proto).
add_library(DarwinCommon
    STATIC
        convert_math.cpp
        convert_math.h
        darwin_constant.h
        stl_proto_wrapper.cpp
        stl_proto_wrapper.h
        world_simulator.cpp
        world_simulator.h
        vector.cpp
        vector.h
        physic.cpp
        physic.h
        darwin_service.proto
        client_audio.proto
        client_parameter.proto
        vector_math.proto
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
    darwin_service.pb.h
    darwin_service.grpc.pb.h
    client_audio.pb.h
    client_audio.grpc.pb.h
    client_parameter.pb.h
    client_parameter.grpc.pb.h
    vector_math.pb.h
    vector_math.grpc.pb.h
)

foreach(HEADERN ${HEADERS_TO_BE_COPIED})
    add_custom_command(
        TARGET DarwinCommon
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/Common/${HEADERN}
            ${CMAKE_SOURCE_DIR}/Common/
    )
endforeach(HEADERN)

set_property(TARGET DarwinCommon PROPERTY FOLDER "DarwinCommon")
