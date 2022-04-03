add_library(thread_pool SHARED thread_pool.cpp)
target_link_libraries(thread_pool PUBLIC pthread)