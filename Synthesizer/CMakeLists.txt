cmake_minimum_required(VERSION 3.29.5)

# Sets a few variables, like PROJECT_NAME
project(Synthesizer)

set(CMAKE_CXX_STANDARD 20)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Downloads CPM if not already downloaded.
include(cmake/cpm.cmake)

# This commands downloads AND configures JUCE.
CPMAddPackage(
    NAME JUCE
    GIT_TAG 8.0.0
    VERSION 8.0.0
    GITHUB_REPOSITORY juce-framework/JUCE
    SOURCE_DIR ${LIB_DIR}/juce
)

# Adds googletest.
CPMAddPackage(
    NAME GOOGLETEST
    GITHUB_REPOSITORY google/googletest
    GIT_TAG v1.14.0
    VERSION 1.14.0
    SOURCE_DIR ${LIB_DIR}/googletest
    OPTIONS
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt ON"
)

# This command allows running tests from the "build" folder.
enable_testing()

# Adds all the targets configured in the "plugin" folder.
add_subdirectory(plugin)

# Adds all the targets configured in the "test" folder.
add_subdirectory(test)
