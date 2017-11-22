cmake_minimum_required(VERSION 3.0)

project(QSignalInspectorTests)

enable_testing()

set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Test)

include_directories(${PROJECT_SOURCE_DIR}/..)

add_executable(QSignalInspectorTest QSignalInspectorTest.cpp "${PROJECT_SOURCE_DIR}/../QSignalInspector.h")
target_link_libraries(QSignalInspectorTest Qt5::Core Qt5::Test)
add_test(NAME QSignalInspectorTest COMMAND QSignalInspectorTest)