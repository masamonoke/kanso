cmake_minimum_required(VERSION 3.22)

set(NAME kanso)
project(${NAME})

set(CMAKE_C_STANDARD 11)

add_subdirectory(deps/glfw)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(include)

find_package(OpenGL REQUIRED)
message(STATUS "Found OpenGL")

find_package(glfw3 3.3 REQUIRED)
message(STATUS "Found GLFW")

file(GLOB SRC
     "src/*.c"
)

add_executable(${NAME} ${SRC})

target_link_libraries(${NAME} glfw OpenGL::GL)
