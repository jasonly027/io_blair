add_executable(${PROJECT_NAME}_test
    example_test.cpp
)

target_link_libraries(${PROJECT_NAME}_test PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests("${PROJECT_NAME}_test")
