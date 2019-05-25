cmake_minimum_required(VERSION 3.0)
project(QProgressIndicator LANGUAGES CXX)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

add_library(QProgressIndicator STATIC "${CMAKE_CURRENT_SOURCE_DIR}/QProgressIndicator.cpp")

set_property(TARGET QProgressIndicator APPEND
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")
set_property(TARGET QProgressIndicator PROPERTY CXX_STANDARD 14)

target_link_libraries(QProgressIndicator Qt5::Core Qt5::Widgets)
