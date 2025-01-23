add_executable(${PROJECT_NAME}_test
  json_test.cpp
  session_view_test.cpp
  lobby_data_test.cpp
)
target_include_directories(${PROJECT_NAME}_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mock
)

target_link_libraries(${PROJECT_NAME}_test PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests("${PROJECT_NAME}_test")
