cmake_minimum_required(VERSION 3.10)

# Example executable
add_executable(example_usage example_usage.cpp)

# Link against the json2doc library
target_link_libraries(example_usage PRIVATE json2doc)
