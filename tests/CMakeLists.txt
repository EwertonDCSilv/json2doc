cmake_minimum_required(VERSION 3.10)

# Simple test executable
add_executable(test_json2doc test_json2doc.cpp)

# Link against the json2doc library
target_link_libraries(test_json2doc PRIVATE json2doc)

# Add test
add_test(NAME test_json2doc COMMAND test_json2doc)
