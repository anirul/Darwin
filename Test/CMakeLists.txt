# Darwin Test

add_executable(DarwinTest
    main.cpp
)

target_include_directories(DarwinTest
    PUBLIC
        ${CMAKE_SOURCE_DIR}/Test
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(DarwinTest
    PUBLIC
        DarwinCommon
        GTest::gmock
        GTest::gtest
)

set_property(TARGET DarwinTest PROPERTY FOLDER "DarwinTest")
