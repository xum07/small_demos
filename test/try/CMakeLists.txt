aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)
add_executable(try_test ${SRC_FILES})
target_include_directories(try_test PRIVATE ${PROJECT_SOURCE_DIR}/src/try)
target_link_libraries(try_test PRIVATE
  ${GTEST_BOTH_LIBRARIES}
  GTest::gmock
  GTest::gmock_main
  try
)

add_test(try_test COMMAND try_test)