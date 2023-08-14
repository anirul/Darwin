# Darwin client.

add_executable(DarwinServer
    darwin_service_impl.cpp
    darwin_service_impl.h
    main.cpp
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
