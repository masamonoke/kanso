#pragma once

#include <memory>

namespace kanso {

	class window;

	class gui {
		public:
			gui(const std::shared_ptr<window> &w);
			virtual ~gui() = default;

			virtual void draw();

		private:
			virtual void create_frame() = 0;
	};

	class opengl_gui : public gui {
		public:
			opengl_gui(const std::shared_ptr<window> &w);
			~opengl_gui() override;

			void draw() override;

		private:
			void create_frame() override;
	};
}
