add_executable(client_test
    ClientWapperTest.cpp
)
target_link_libraries(client_test
    client
)

add_dependencies(client_test client)