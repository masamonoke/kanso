#pragma once

#include <string>

#include "window.hpp"
#include "loader.hpp"
#include "event_system.hpp"

#include <nlohmann/json.hpp>

namespace kanso {
	class app {
		public:
			explicit app(nlohmann::json&& json);

			void update();
			void input();

			[[nodiscard]] bool close() const {
				return close_;
			}

		private:
			bool                          close_;
			std::shared_ptr<window>       window_;
			std::unique_ptr<loader>       loader_;
			std::unique_ptr<scene>        scene_;
			std::shared_ptr<camera>       camera_;
			std::unique_ptr<event_system> event_system_;
	};
} // namespace kanso
