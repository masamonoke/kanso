#pragma once

#include <cmath>
#include <fstream>

#include "model.hpp"
#include "light.hpp"
#include "thread_pool.hpp"
#include "scene.hpp"
#include "camera.hpp"

#include <nlohmann/json.hpp>

namespace kanso {

	namespace exception {

		class bad_scene_file_exception : public base_kanso_exception {
			public:
				bad_scene_file_exception(std::string&& msg) : base_kanso_exception(std::move(msg)) {}
		};

		class camera_not_found_exception : public base_kanso_exception {
			public:
				camera_not_found_exception(std::string&& msg) : base_kanso_exception(std::move(msg)) {}
		};

	} // namespace exception

	class loader {
		public:
			loader(nlohmann::json&& json);

			std::unique_ptr<scene>  make_scene();
			std::shared_ptr<camera> make_camera();

		private:
			void init_load();

			nlohmann::json                                     json_;
			std::map<std::string, std::shared_ptr<model_data>> models_;

			void load_models_data(const nlohmann::json& models_json);
			void load_models(const nlohmann::json&                       models_json,
			                 std::vector<std::shared_ptr<loaded_model>>& loaded_models);
	};

} // namespace kanso
