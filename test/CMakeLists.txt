ENABLE_TESTING()

set(GTEST_ROOT ${PROJECT_SOURCE_DIR}/opensource/googletest)
find_package(GTest REQUIRED)
include(${GTEST_ROOT}/lib/cmake/GTest/GTestConfig.cmake)
include_directories(${GTEST_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(thread_pool_test)