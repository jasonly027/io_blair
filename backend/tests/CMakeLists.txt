add_executable(${PROJECT_NAME}_test
    example_test.cpp
)

target_link_libraries(${PROJECT_NAME}_test PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME}_test PRIVATE _WIN32_WINNT=0x0A00)
endif()

include(GoogleTest)
gtest_discover_tests("${PROJECT_NAME}_test")
