cmake_minimum_required(VERSION 3.0)
project(opengl_test)
find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)
add_executable(opengl_test ${SOURCE_FILES})
target_link_libraries(opengl_test ${OPENGL_LIBRARIES} glfw)