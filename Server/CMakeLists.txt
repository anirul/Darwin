# Darwin client.

add_executable(DarwinServer
    character_info.h
    darwin_service_impl.cpp
    darwin_service_impl.h
    element_info.h
    main.cpp
    update_responder.cpp
    update_responder.h
    update_responder_container.cpp
    update_responder_container.h
    world_state.cpp
    world_state.h
    world_state_file.cpp
    world_state_file.h
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
