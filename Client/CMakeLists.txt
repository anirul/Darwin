# Darwin client.

add_executable(DarwinClient
    WIN32
        main.cpp
        darwin_client.cpp
        darwin_client.h
        world_client.cpp
        world_client.h
        modal_character.cpp
        modal_character.h
        modal_disconnected.cpp
        modal_disconnected.h
        modal_ping.cpp
        modal_ping.h
        modal_server.cpp
        modal_server.h
        state_interface.h
        state_character.cpp
        state_character.h
        state_context.cpp
        state_context.h
        state_disconnected.cpp
        state_disconnected.h
        state_title.cpp
        state_title.h
        state_ping.cpp
        state_ping.h
        state_play.cpp
        state_play.h
        state_server.cpp
        state_server.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../asset/json/darwin_client.json
        ${CMAKE_CURRENT_SOURCE_DIR}/../asset/shader/opengl/darwin_ray_marching.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/../asset/shader/opengl/darwin_ray_marching.frag
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
