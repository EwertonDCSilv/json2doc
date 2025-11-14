cmake_minimum_required(VERSION 3.10)
project(json2doc VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library source files
set(SOURCES
    src/json2doc.cpp
    src/converter.cpp
)

# Library header files
set(HEADERS
    include/json2doc/json2doc.h
    include/json2doc/converter.h
)

# Create library
add_library(json2doc ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(json2doc
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Installation rules
include(GNUInstallDirs)

install(TARGETS json2doc
    EXPORT json2docTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets
install(EXPORT json2docTargets
    FILE json2docTargets.cmake
    NAMESPACE json2doc::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/json2doc
)

# Optional: Build examples
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Optional: Build tests
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
