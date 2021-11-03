aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)
ADD_LIBRARY(async_thread STATIC ${SRC_FILES})
target_link_libraries(async_thread PRIVATE pthread)