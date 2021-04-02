project(pipistrellus)
cmake_minimum_required(VERSION 2.8)
set(CPPSTD "c++11")

enable_testing()
add_test(TestLevel1 ${PROJECT_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

aux_source_directory(. SRC_LIST)
list(REMOVE_ITEM SRC_LIST "test_data.c")
message(STATUS "SRC_LIST:\"${SRC_LIST}\"")

message(STATUS "CMAKE_BUILD_TYPE: \"${CMAKE_BUILD_TYPE}\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-all -fstack-check -fstack-protector-strong -fsanitize=bounds -fsanitize=vla-bound -fsanitize=leak -pedantic -fsanitize=address -Wall -Wfatal-errors -g -O0 --std=${CPPSTD}")

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME})
