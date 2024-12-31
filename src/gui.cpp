#include "gui.hpp"
#include "window.hpp"
#ifndef OPENGL_AVAILABLE
#include "exception.hpp"
#endif
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdouble-promotion"

#include "backends/imgui_impl_glfw.h"

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include "backends/imgui_impl_opengl3.h"
#include "spdlog/spdlog.h"

namespace kanso {

	gui::gui(const std::shared_ptr<const window>&, const std::shared_ptr<const scene>& scene) : scene_(scene) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
	}

	void gui::draw() {
		create_frame();
		create_windows();
		ImGui::Render();
	}

	opengl_gui::opengl_gui(const std::shared_ptr<const window>& w, const std::shared_ptr<const scene>& scene)
	    : gui(w, scene),
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
		std::vector<model_view> items= scene_->models();

        ImGui::Begin("Menu");

		const std::string name_title = "Object Name";
        ImGui::InputText("Name", const_cast<char *>(name_title.c_str()), name_title.size()); // NOLINT

        ImGui::Separator();

		int id = 0;
        for (auto& item : items) {
            ImGui::PushID(id++);

            ImGui::Text("%s", item.name.c_str());

            ImGui::InputFloat3("Position", item.pos.data());
            ImGui::InputFloat3("Rotation", item.rot.data());
            ImGui::InputFloat3("Scale", item.scale.data());
            ImGui::InputText("Type", item.type.data(), item.type.capacity());

            ImGui::Separator();
            ImGui::PopID();
        }

        ImGui::End();
	}

	bool opengl_gui::handle_click(void* ctx, enum mouse_button b, enum button_status action) {
		auto* window = static_cast<GLFWwindow*>(ctx);

		const int glfw_button = mouse_buttons_map_[b];
		const int glfw_action = button_actions_map_[action];

		ImGui_ImplGlfw_MouseButtonCallback(window, glfw_button, glfw_action, 0);

		return ImGui::IsAnyItemHovered();

	}

	std::unique_ptr<gui> gui_factory::make_gui(const std::shared_ptr<window>& window, const std::shared_ptr<const scene>& scene) {
#ifdef OPENGL_AVAILABLE
		return std::make_unique<opengl_gui>(window, scene);
#else
		throw exception::not_implemented_exception("Unknown graphics API");
#endif
	}

	bool gui::blocked_keyboard() const {
		return char_input_;
	}

	void opengl_gui::handle_char(uint c) {
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharacter(c);
		char_input_ = io.WantCaptureKeyboard;
	}

} // namespace kanso
