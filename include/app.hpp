#pragma once

#include <string>

#include "window.hpp"
#include "loader.hpp"
#include "event_system.hpp"
#include "gui.hpp"

namespace kanso {
	class app {
		public:
			app(nlohmann::json json);

			// if using opengl this method must be in main thread
			void run();

		private:
			bool                          close_;
			std::shared_ptr<window>       window_;
			std::unique_ptr<loader>       loader_;
			std::shared_ptr<scene>        scene_;
			std::shared_ptr<camera>       camera_;
			std::unique_ptr<event_system> event_system_;
			std::unique_ptr<renderer>     renderer_;
			std::unique_ptr<gui>          gui_;

			void update();
			void input();

			[[nodiscard]] bool close() const {
				return close_;
			}
	};
} // namespace kanso
