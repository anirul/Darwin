# Darwin Server Test

add_executable(DarwinServerTest
    ${CMAKE_SOURCE_DIR}/Server/world_state.cpp
    ${CMAKE_SOURCE_DIR}/Server/world_state.h
    ${CMAKE_SOURCE_DIR}/Server/world_state_file.cpp
    ${CMAKE_SOURCE_DIR}/Server/world_state_file.h
    main.cpp
    world_state_test.cpp
    world_state_test.h
    world_state_file_test.cpp
    world_state_file_test.h
)

target_include_directories(DarwinServerTest
    PUBLIC
        ${CMAKE_SOURCE_DIR}/Test
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(DarwinServerTest
    PUBLIC
        DarwinCommon
        GTest::gmock
        GTest::gtest
)

set_property(TARGET DarwinServerTest PROPERTY FOLDER "DarwinTest")

gtest_discover_tests(DarwinServerTest)
