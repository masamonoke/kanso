#pragma once

#include <memory>

#include "core.hpp"

namespace kanso {

	class window;

	class gui {
		public:
			gui(const std::shared_ptr<window>& w);
			virtual ~gui() = default;

			virtual void draw();
			virtual void handle_click(void*, enum mouse_button b, enum button_status action) = 0;

		private:
			virtual void create_frame() = 0;
	};

	class opengl_gui : public gui {
		public:
			opengl_gui(const std::shared_ptr<window>& w);
			~opengl_gui() override;

			void draw() override;
			void handle_click(void* ctx, enum mouse_button b, enum button_status action) override;

		private:
			std::map<enum mouse_button, int>  mouse_buttons_map_;
			std::map<enum key_button, int>    key_buttons_map_;
			std::map<enum button_status, int> button_actions_map_;

			void create_frame() override;
	};

	namespace gui_factory {
		std::unique_ptr<gui> make_gui(const std::shared_ptr<window>&);
	};
} // namespace kanso
