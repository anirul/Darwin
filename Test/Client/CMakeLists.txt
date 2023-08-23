# Darwin Server Test

add_executable(DarwinClientTest
    main.cpp
)

target_include_directories(DarwinClientTest
    PUBLIC
        ${CMAKE_SOURCE_DIR}/Test
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(DarwinClientTest
    PUBLIC
        DarwinCommon
        GTest::gmock
        GTest::gtest
)

set_property(TARGET DarwinClientTest PROPERTY FOLDER "DarwinTest")
