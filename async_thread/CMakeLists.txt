project(async_thread)
cmake_minimum_required(VERSION 3.10)

add_compile_options(-Wall -Werror -O0 -g)
set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS}")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/../utils/utils.cpp)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../utils)
target_link_libraries(async_thread PRIVATE pthread)