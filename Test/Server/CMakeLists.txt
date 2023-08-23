# Darwin Server Test

add_executable(DarwinServerTest
    main.cpp
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
