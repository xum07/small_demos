# test thread_pool
ADD_LIBRARY(sync_main STATIC main.cpp)
target_link_libraries(sync_main PRIVATE pthread)

# test async thread_manager
ADD_LIBRARY(async_main STATIC async_main.cpp)
target_link_libraries(async_main PRIVATE pthread)