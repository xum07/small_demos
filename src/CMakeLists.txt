INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/utils)

# ADD_SUBDIRECTORY(async_thread)
# ADD_SUBDIRECTORY(mmap_demo)
# ADD_SUBDIRECTORY(pg_vfd_demo)
# ADD_SUBDIRECTORY(state_machine)
# ADD_SUBDIRECTORY(thread_pool_demo)
ADD_SUBDIRECTORY(try)

ADD_LIBRARY(${CMAKE_PROJECT_NAME} utils/utils.cpp)
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} PRIVATE
  # subdirectory libs
#   async_thread
#   mmap_demo
#   pg_vfd_demo
#   state_machine
#   thread_pool_demo
  try
)
