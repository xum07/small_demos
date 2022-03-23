cmake_minimum_required(VERSION 3.14)
PROJECT(small_demos VERSION 0.1.0)

# sometimes we need version 20 to write codes with thread
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations -Wall -Werror -O0 -g")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(test)