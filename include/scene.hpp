#pragma once

#include "model.hpp"
#include "light.hpp"
#include "camera.hpp"
#include "window.hpp"

namespace kanso {

	class scene {
		public:
			scene(std::vector<std::shared_ptr<model>> models, std::vector<std::shared_ptr<light>> lights);

			void draw(const camera& camera, const window& window);

			void add_model(std::unique_ptr<model> model);

			std::vector<std::shared_ptr<model>>::iterator begin() {
				return models_.begin();
			}

			std::vector<std::shared_ptr<model>>::iterator end() {
				return models_.end();
			}

			std::vector<model_view> models() const;

		private:
			std::vector<std::shared_ptr<model>> models_;
			std::vector<std::shared_ptr<light>> lights_;
	};

} // namespace kanso
