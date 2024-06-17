include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

cmake_minimum_required(VERSION 3.23.2)

set(NAME kanso)
project(${NAME})

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(deps/glfw)
add_subdirectory(deps/cglm)
add_subdirectory(deps/assimp)
add_subdirectory(deps/json-c)
set(ASSIMP_BUILD_STATIC_LIB ON)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(include)
include_directories(deps/log.c/src)
include_directories(deps/assimp/include)
include_directories(deps/json-c)

find_package(OpenGL REQUIRED)
message(STATUS "Found OpenGL")

file(GLOB SRC
     "src/*.c"
     "src/*.cpp"
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

add_custom_target(
	cfg DEPENDS ${CFG_FILES}
)
add_dependencies(${PROJECT_NAME} cfg)
add_custom_command(TARGET cfg POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/cfg ${CMAKE_BINARY_DIR}/cfg
)

find_program(iwyu_path NAMES include-what-you-use iwyu)
if (iwyu_path)
	message(STATUS "Using include-what-you-see")
	set_property(TARGET ${NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
	set_property(TARGET ${NAME} PROPERTY C_INCLUDE_WHAT_YOU_USE ${iwyu_path})
endif()


if (NOT MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE -Waddress -Wall -Warray-bounds -Wbool-operation -Wchar-subscripts -Wcomment
		-Wmisleading-indentation -Wparentheses -Wextra -pedantic -Wstrict-prototypes -Wshadow -Wconversion -Wvla -Wdouble-promotion -Wmissing-noreturn
		-Wmissing-format-attribute -Wmissing-prototypes -Wunused-result)
	if (NOT WIN32 AND ${CMAKE_BUILD_TYPE} MATCHES Debug)
		message(STATUS "Using fsanitize")
		target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined,null,bounds,leak)
		target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG=ON)
	endif()
endif()

target_link_libraries(${PROJECT_NAME} glfw OpenGL::GL cglm assimp json-c)

