#include "gui.hpp"
#include "window.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdouble-promotion"

#include "backends/imgui_impl_glfw.h"

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include "backends/imgui_impl_opengl3.h"
#include "spdlog/spdlog.h"

namespace kanso {

	gui::gui(const std::shared_ptr<window> &) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
	}

	void gui::draw() {
		create_frame();
		ImGui::Render();
	}

	opengl_gui::opengl_gui(const std::shared_ptr<window> &w) : gui(w) {
		const char* glsl_version = "#version 410 core";
		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(w->internal()), true);
		ImGui_ImplOpenGL3_Init(glsl_version);
		ImGui::StyleColorsDark();
		spdlog::info("User interface is initialized");
	}

	opengl_gui::~opengl_gui() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void opengl_gui::draw() {
		gui::draw();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void opengl_gui::create_frame() {

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
	}
}
