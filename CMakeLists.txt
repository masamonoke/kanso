include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

cmake_minimum_required(VERSION 3.23.2)

set(NAME kanso)
project(${NAME})

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(deps/glfw)
add_compile_definitions(USING_GLFW)
add_subdirectory(deps/assimp)
add_subdirectory(deps/spdlog)
add_subdirectory(deps/json)
add_subdirectory(deps/glm)
set(ASSIMP_BUILD_STATIC_LIB ON)

include_directories(include)
include_directories(deps/assimp/include)
include_directories(deps/json/include)
include_directories(deps/glm)
include_directories(deps/imgui)

find_package(OpenGL REQUIRED)
message(STATUS "Found OpenGL")

file(GLOB SHADERS_OPENGL
	shaders/*
)

file(GLOB IMGUI
	deps/imgui/imgui_draw.cpp
	deps/imgui/backends/imgui_impl_glfw.cpp
	deps/imgui/imgui_tables.cpp
	deps/imgui/imgui_widgets.cpp
	deps/imgui/imgui.cpp
	deps/imgui/backends/imgui_impl_opengl3.cpp
	deps/imgui/imgui_demo.cpp
)

add_executable(${PROJECT_NAME}
	src/app.cpp
	src/camera.cpp
	src/event_system.cpp
	src/glad.c
	src/light.cpp
	src/loader.cpp
	src/main.cpp
	src/mesh.cpp
	src/loaded_model.cpp
	src/scene.cpp
	src/shader.cpp
	src/texture.cpp
	src/window.cpp
	src/model_data_loader.cpp
	src/renderer.cpp
	src/starter.cpp
	src/raycast.cpp
	src/primitive.cpp
	src/gui.cpp
	src/core.cpp
	src/input_system.cpp
	src/object_manager.cpp
	${IMGUI}
)

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

add_custom_target(
	scene DEPENDS ${SCENE_FILES}
)
add_dependencies(${PROJECT_NAME} scene)
add_custom_command(TARGET scene POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/scene ${CMAKE_BINARY_DIR}/scene
)

if (NOT MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE -Waddress -Wall -Warray-bounds -Wbool-operation -Wchar-subscripts -Wcomment
		-Wmisleading-indentation -Wparentheses -Wextra -pedantic -Wstrict-prototypes -Wshadow -Wconversion -Wvla -Wdouble-promotion -Wmissing-noreturn
		-Wmissing-format-attribute -Wmissing-prototypes -Wunused-result)
endif()

if (${CMAKE_BUILD_TYPE} MATCHES Debug)
	if (NOT WIN32)
		if (APPLE)
			message(STATUS "Using fsanitize")
			target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
		else()
			message(STATUS "Using fsanitize")
			target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined,leak)
		endif()
	endif()
	target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG=ON)
endif()

target_link_libraries(${PROJECT_NAME} glfw OpenGL::GL assimp spdlog nlohmann_json::nlohmann_json glm)
