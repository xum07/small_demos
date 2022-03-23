ENABLE_TESTING()

set(GTEST_ROOT ${PROJECT_SOURCE_DIR}/opensource/googletest)
find_package(GTest REQUIRED)
include(${GTEST_ROOT}/lib/cmake/GTest/GTestConfig.cmake)
include_directories(${GTEST_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/src)

add_subdirectory(try)