# Darwin client.

add_executable(DarwinServer
    WIN32
        main.cpp
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
