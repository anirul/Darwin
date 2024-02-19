# Darwin client.

add_executable(DarwinServer
    compute_world.cpp
    compute_world.h
    convert_math.cpp
    convert_math.h
    darwin_service_impl.cpp
    darwin_service_impl.h
    element_info.h
    main.cpp
    physic_engine.cpp
    physic_engine.h
    character_info.h
    user_state.cpp
    user_state.h
    user_state_file.cpp
    user_state_file.h
    world_state.cpp
    world_state.h
    world_state_file.cpp
    world_state_file.h
    user_db.json
    world_db.json
)

target_include_directories(DarwinServer
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(DarwinServer
    PUBLIC
        absl::flags_parse
        DarwinCommon
)

set_property(TARGET DarwinServer PROPERTY FOLDER "DarwinServer")
