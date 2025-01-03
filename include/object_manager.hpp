#pragma once

#include <vector>

namespace kanso {

	class model;
	class light;
	struct model_view;

	class object_manager {
		public:
			object_manager(std::vector<std::shared_ptr<model>> models, std::vector<std::shared_ptr<light>> lights);

			void add_model(std::unique_ptr<model> model);

			std::vector<std::shared_ptr<model>>::const_iterator model_begin() {
				return models_.begin();
			}

			std::vector<std::shared_ptr<model>>::const_iterator model_end() {
				return models_.end();
			}

			std::vector<std::shared_ptr<light>>::const_iterator light_begin() {
				return lights_.begin();
			}

			std::vector<std::shared_ptr<light>>::const_iterator light_end() {
				return lights_.end();
			}

			std::vector<model_view>::iterator view_begin() {
				return model_views_.begin();
			}

			std::vector<model_view>::iterator view_end() {
				return model_views_.end();
			}

		private:
			std::vector<std::shared_ptr<model>> models_;
			std::vector<std::shared_ptr<light>> lights_;
			std::vector<model_view> model_views_;
	};

}
