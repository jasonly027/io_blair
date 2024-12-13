# Library
add_library(${PROJECT_NAME}_lib STATIC
    server.cpp
    session.cpp
    game.cpp
    lobby.cpp
    response.cpp
)
target_include_directories(${PROJECT_NAME}_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(${PROJECT_NAME}_lib PUBLIC
    Boost::beast
    simdjson
    reflectcpp
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME}_lib PUBLIC _WIN32_WINNT=0x0A00)
endif()

# Executable
add_executable(${PROJECT_NAME}
    main.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)
