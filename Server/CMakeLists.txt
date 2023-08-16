# Darwin client.

add_executable(DarwinServer
    convert_math.cpp
    convert_math.h
    darwin_service_impl.cpp
    darwin_service_impl.h
    element_info.h
    main.cpp
    physic_engine.cpp
    physic_engine.h
    player_info.h
    world_state.cpp
    world_state.h
)

target_include_directories(DarwinServer
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(DarwinServer
    PUBLIC
        DarwinCommon
)

set_property(TARGET DarwinServer PROPERTY FOLDER "DarwinServer")
