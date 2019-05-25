cmake_minimum_required(VERSION 3.0)

project(score_base LANGUAGES CXX)
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory(lib)

add_subdirectory(plugins)

add_subdirectory(app)
