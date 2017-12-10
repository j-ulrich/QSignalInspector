cmake_minimum_required(VERSION 3.0)

project(QSignalInspectorTests)

enable_testing()

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 COMPONENTS Core Test OPTIONAL_COMPONENTS Network)

include_directories(${PROJECT_SOURCE_DIR}/..)

add_executable(QSignalInspectorTest QSignalInspectorTest.cpp "${PROJECT_SOURCE_DIR}/../QSignalInspector.h")
target_link_libraries(QSignalInspectorTest Qt5::Core Qt5::Test)
add_test(NAME QSignalInspectorTest COMMAND QSignalInspectorTest)

if(Qt5Network_FOUND)
	add_executable(QNetworkReplyBehaviorTest QNetworkReplyBehaviorTest.cpp "${PROJECT_SOURCE_DIR}/../QSignalInspector.h")
	target_link_libraries(QNetworkReplyBehaviorTest Qt5::Core Qt5::Test Qt5::Network)
	set_target_properties(QNetworkReplyBehaviorTest PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED ON
	)
endif()