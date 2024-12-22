#pragma once

#include <memory>

#include "core.hpp"
#include "scene.hpp"

namespace kanso {

	class window;

	class gui {
		public:
			gui(const std::shared_ptr<const window>& w, const std::shared_ptr<const scene>& scene);
			virtual ~gui() = default;

			virtual void draw();
			virtual bool handle_click(void*, enum mouse_button b, enum button_status action) = 0;

			void toggle_draw() { enable_draw_ = !enable_draw_; }

		private:
			virtual void create_frame() = 0;
			virtual void create_windows() = 0;

		protected:
			bool enable_draw_ = false;
			std::shared_ptr<const scene> scene_;
	};

	class opengl_gui : public gui {
		public:
			opengl_gui(const std::shared_ptr<const window>& w, const std::shared_ptr<const scene>& scene);
			~opengl_gui() override;

			void draw() override;
			bool handle_click(void* ctx, enum mouse_button b, enum button_status action) override;

		private:
			std::map<enum mouse_button, int>  mouse_buttons_map_;
			std::map<enum key_button, int>    key_buttons_map_;
			std::map<enum button_status, int> button_actions_map_;

			void create_frame() override;
			void create_windows() override;
	};

	namespace gui_factory {
		std::unique_ptr<gui> make_gui(const std::shared_ptr<window>& window, const std::shared_ptr<const scene>& scene);
	};
} // namespace kanso
