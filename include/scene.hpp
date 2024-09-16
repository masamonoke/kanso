#pragma once

#include "model.hpp"
#include "light.hpp"
#include "camera.hpp"
#include "window.hpp"
#include "exception.hpp"

namespace kanso {

	class scene {
		public:
			scene(std::vector<std::shared_ptr<loaded_model>> models, std::vector<std::shared_ptr<light>> lights);

			void draw(const camera& camera, const window& window);

		private:
			std::vector<std::shared_ptr<loaded_model>> models_;
			std::vector<std::shared_ptr<light>>        lights_;
	};

} // namespace kanso
