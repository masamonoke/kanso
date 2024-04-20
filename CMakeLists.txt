include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

cmake_minimum_required(VERSION 3.28.0)

set(NAME kanso)
project(${NAME})

set(CMAKE_C_STANDARD 17)

add_subdirectory(deps/glfw)
add_subdirectory(deps/cglm)

include_directories(${OPENGL_INCLUDE_DIRS})

include_directories(include)
include_directories(deps/log.c/src)

find_package(OpenGL REQUIRED)
message(STATUS "Found OpenGL")

find_package(glfw3 REQUIRED)
message(STATUS "Found GLFW3")

find_package(glfw3 REQUIRED)
message(STATUS "Found GLM")

file(GLOB SRC
     "src/*.c"
	 "include/*.h"
	 deps/log.c/src/log.c
)

file(GLOB SHADERS_OPENGL
	shaders/*
)
add_executable(${PROJECT_NAME} ${SRC})

add_custom_target(
	shaders DEPENDS ${SHADERS_OPENGL}
)
add_dependencies(${PROJECT_NAME} shaders)
add_custom_command(TARGET shaders POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders
)

add_custom_target(
	assets DEPENDS ${ASSET_FILES}
)
add_dependencies(${PROJECT_NAME} assets)
add_custom_command(TARGET assets POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
)

target_compile_options(${PROJECT_NAME} PRIVATE -Waddress -Wall -Warray-bounds -Wbool-operation -Wchar-subscripts -Wcomment -Wmisleading-indentation -Wparentheses -Wextra)
target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined,null,bounds)

target_link_libraries(${PROJECT_NAME} glfw OpenGL::GL cglm)
