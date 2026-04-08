add_executable(find_magics find_magics.cpp)
target_include_directories(find_magics PRIVATE ../src)
target_link_libraries(find_magics PRIVATE cheslib)
