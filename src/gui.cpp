#include "gui.hpp"
#include "window.hpp"
#include "exception.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdouble-promotion"

#include "backends/imgui_impl_glfw.h"

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include "backends/imgui_impl_opengl3.h"
#include "spdlog/spdlog.h"

namespace kanso {

	gui::gui(const std::shared_ptr<window>&) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
	}

	void gui::draw() {
		create_frame();
		create_windows();
		ImGui::Render();
	}

	opengl_gui::opengl_gui(const std::shared_ptr<window>& w)
	    : gui(w),
	      mouse_buttons_map_(mapped_mouse_buttons()),
	      key_buttons_map_(mapped_keys()),
		  button_actions_map_(mapped_actions())
	{
		const char* glsl_version = "#version 410 core";
		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(w->internal()), false);
		ImGui_ImplOpenGL3_Init(glsl_version);
		spdlog::info("User interface is initialized");
	}

	opengl_gui::~opengl_gui() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void opengl_gui::draw() {
		if (enable_draw_) {
			gui::draw();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
	}

	void opengl_gui::create_frame() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void opengl_gui::create_windows() {
		static float sliderValue = 0.5f;
		static bool checkboxState = false;
		static int buttonClickCount = 0;
		ImGui::Begin("My Custom Window");
        ImGui::Text("Hello, world!");

        // Slider
        ImGui::SliderFloat("Slider", &sliderValue, 0.0f, 1.0f, "Value: %.3f");

        // Checkbox
        ImGui::Checkbox("Check Me", &checkboxState);
        ImGui::Text("Checkbox is %s", checkboxState ? "ON" : "OFF");

        // Button
        if (ImGui::Button("Click Me")) {
            buttonClickCount++;
        }
        ImGui::SameLine();
        ImGui::Text("Clicked %d times", buttonClickCount);

        ImGui::End();
	}

	void opengl_gui::handle_click(void* ctx, enum mouse_button b, enum button_status action) {
		auto* window = static_cast<GLFWwindow*>(ctx);

		const int glfw_button = mouse_buttons_map_[b];
		const int glfw_action = button_actions_map_[action];

		ImGui_ImplGlfw_MouseButtonCallback(window, glfw_button, glfw_action, 0);
	}

	std::unique_ptr<gui> gui_factory::make_gui(const std::shared_ptr<window>& window) {
#ifdef OPENGL_AVAILABLE
		return std::make_unique<opengl_gui>(window);
#else
		throw exception::not_implemented_exception("Unknown graphics API");
#endif
	}

} // namespace kanso
