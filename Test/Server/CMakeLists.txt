# Darwin Server Test

add_executable(DarwinServerTest
    ${CMAKE_SOURCE_DIR}/Server/convert_math.cpp
    ${CMAKE_SOURCE_DIR}/Server/convert_math.h
    ${CMAKE_SOURCE_DIR}/Server/physic_engine.cpp
    ${CMAKE_SOURCE_DIR}/Server/physic_engine.h
    ${CMAKE_SOURCE_DIR}/Server/world_state.cpp
    ${CMAKE_SOURCE_DIR}/Server/world_state.h
    main.cpp
    world_state_test.cpp
    world_state_test.h
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
