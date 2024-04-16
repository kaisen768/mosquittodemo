cmake_minimum_required(VERSION 3.14)

project(mosquittodemo LANGUAGES CXX)

find_package(mosquitto REQUIRED)

include_directories(${MOSQUITTO_INCLUDE_DIRS})

file(GLOB_RECURSE source_codes main.cc mosquittodemo.cc)

add_executable(${project_name} ${source_codes})
target_link_libraries(${project_name} ${MOSQUITTOPP_LIBRARIES} pthread)
