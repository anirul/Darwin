# Darwin client.

add_executable(DarwinClient
    WIN32
        main.cpp
        darwin_client.json
)

target_include_directories(DarwinClient
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(DarwinClient
    PUBLIC
        Frame
        FrameCommon
        FrameOpenGL
        FrameProto
)

set_property(TARGET DarwinClient PROPERTY FOLDER "DarwinClient")
