project(pg_vfd_demo)
cmake_minimum_required(VERSION 3.10)

add_compile_options(-Wall -Werror -O0 -g)
set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS}")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/../utils/utils.cpp)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../utils)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/files DESTINATION ${CMAKE_CURRENT_BINARY_DIR})