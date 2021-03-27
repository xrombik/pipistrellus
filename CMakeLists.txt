project(pipistrellus)
cmake_minimum_required(VERSION 2.8)
set(CPPSTD "c++11")

enable_testing()
add_test(TestLevel1 ${PROJECT_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

aux_source_directory(. SRC_LIST)
message(STATUS "SRC_LIST:\"${SRC_LIST}\"")

message(STATUS "CMAKE_BUILD_TYPE: \"${CMAKE_BUILD_TYPE}\"")
if (CMAKE_BUILD_TYPE STREQUAL Release)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -fpic -pedantic -Wall -Wfatal-errors -s -Os --std=${CPPSTD}")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-all -fstack-check -fstack-protector-strong -fsanitize=bounds -fsanitize=vla-bound -fsanitize=leak -pedantic -fsanitize=address -Wall -Wfatal-errors -g -O0 --std=${CPPSTD}")
endif()

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${FREETYPE_LIBRARIES} ${PCAP_LIBRARY} ${GLFW_LIBRARY} ${PNG_LIBRARY})
