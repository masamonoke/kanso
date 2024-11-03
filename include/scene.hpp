#pragma once

#include "model.hpp"
#include "light.hpp"
#include "camera.hpp"
#include "window.hpp"
#include "exception.hpp"

namespace kanso {

	class scene {
		public:
			scene(std::vector<std::shared_ptr<model>> models, std::vector<std::shared_ptr<light>> lights);

			void draw(const camera& camera, const window& window);

			void add_model(std::unique_ptr<model> model);

			[[nodiscard]] std::vector<std::shared_ptr<model>>::iterator begin() {
				return models_.begin();
			}

			[[nodiscard]] std::vector<std::shared_ptr<model>>::iterator end() {
				return models_.end();
			}

		private:
			std::vector<std::shared_ptr<model>> models_;
			std::vector<std::shared_ptr<light>> lights_;
	};

} // namespace kanso
