add_executable(${PROJECT_NAME}
    main.cpp
    server.cpp
    session.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::beast)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0A00)
endif()
