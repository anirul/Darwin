# Darwin client.

add_executable(DarwinClient
    WIN32
        main.cpp
        darwin_client.cpp
        darwin_client.h
        world_client.cpp
        world_client.h
        network_app.cpp
        network_app.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../asset/json/darwin_client.json
)

target_include_directories(DarwinClient
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(DarwinClient
    PUBLIC
        Frame
        FrameGui
        FrameCommon
        FrameOpenGL
        FrameOpenGLGui
        FrameProto
        DarwinCommon
)

set_property(TARGET DarwinClient PROPERTY FOLDER "DarwinClient")
