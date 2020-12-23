project(thread_pool_demo)
cmake_minimum_required(VERSION 3.10)

add_compile_options(-Wall -Werror -O0 -g)
set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS}")

# test thread_pool
add_executable(main main.cpp ../utils/utils.cpp)
target_include_directories(main PRIVATE ../utils)
target_link_libraries(main PRIVATE pthread)

# test async thread_manager
add_executable(async_main async_main.cpp ../utils/utils.cpp)
target_include_directories(async_main PRIVATE ../utils)
target_link_libraries(async_main PRIVATE pthread)