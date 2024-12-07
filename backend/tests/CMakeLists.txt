add_executable(${PROJECT_NAME}_test
    example_test.cpp
    response_test.cpp
    game_test.cpp
    lobby_test.cpp
)

target_link_libraries(${PROJECT_NAME}_test PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests("${PROJECT_NAME}_test")
