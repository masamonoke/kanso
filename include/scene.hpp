#pragma once

#include "camera.hpp"
#include "object_manager.hpp"

namespace kanso {

	class scene {
		public:
			scene(std::shared_ptr<object_manager> manager);

			void draw(const camera& camera, const window& window);

			void add_model(std::unique_ptr<model> model);

			std::vector<std::shared_ptr<model>>::const_iterator model_begin();
			std::vector<std::shared_ptr<model>>::const_iterator model_end();

			std::vector<model_view>::iterator view_begin() const;
			std::vector<model_view>::iterator view_end() const;

		private:
			std::shared_ptr<object_manager> obj_manager_;
	};

} // namespace kanso
