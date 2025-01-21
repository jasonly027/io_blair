add_library(${PROJECT_NAME}_lib STATIC
    server.cpp
    json.cpp
    string_hash.cpp

    session/session.cpp
    session/session_context.cpp
    session/session_view.cpp

    handler/ihandler.cpp
    handler/handler.cpp
    handler/igame.cpp

    lobby/lobby_data.cpp
    lobby/lobby_manager.cpp
    lobby/lobby_context.cpp
)
target_include_directories(${PROJECT_NAME}_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/session
    ${CMAKE_CURRENT_SOURCE_DIR}/handler
    ${CMAKE_CURRENT_SOURCE_DIR}/lobby
)
target_link_libraries(${PROJECT_NAME}_lib PUBLIC
    Boost::beast
    reflectcpp
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME}_lib PUBLIC _WIN32_WINNT=0x0A00)
endif()

add_executable(${PROJECT_NAME}
    main.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)
