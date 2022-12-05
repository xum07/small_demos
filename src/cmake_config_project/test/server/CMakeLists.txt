add_executable(server_test
    ServerWapperTest.cpp
)
target_link_libraries(server_test
    server
)
add_dependencies(server_test server)