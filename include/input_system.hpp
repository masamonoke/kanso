#pragma once

#include <functional>
#include <future>

#include "core.hpp"

// TODO: enable if USE_GLFW defined
#include "GLFW/glfw3.h"

namespace kanso {

	class input_system {
		public:
			virtual ~input_system() = default;

			virtual void bind_key(enum key_button key, enum key_button_mod mod, enum press_type press,
								  const std::function<void(void)>& f) = 0;

			void handle_input();

		protected:
			using btn_to_func = std::map<std::pair<enum key_button, enum press_type>, std::function<void(void)>>;

			btn_to_func funcs_;
	};

	// TODO: enable if USE_GLFW defined
	class glfw_input final : public input_system {
		public:
			glfw_input(GLFWwindow* window, int double_press_timeout_ms = 1000);

			void bind_key(enum key_button key, enum key_button_mod mod, enum press_type press,
						  const std::function<void(void)>& f) override;

		private:
			GLFWwindow*                       window_;
			std::map<enum key_button, int>    keys_;
			std::map<enum button_status, int> actions_;

			struct double_press_data {
				int press_count;
				long long unix_timestamp;
				std::future<void> future;
			};
			std::map<int, double_press_data> key_press_map_;
			int double_press_timeout_ms_;

			std::function<void(void)> make_press_func(int glfw_key, const std::function<void(void)>& f);
			std::function<void(void)> make_single_press_func(int glfw_key, const std::function<void(void)>& f);
			std::function<void(void)> make_double_press_func(int glfw_key, const std::function<void(void)>& f);
			void countdown(int glfw_key, int ms);
	};
} // namespace kanso
