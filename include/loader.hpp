#pragma once

#include <cmath>
#include <fstream>

#include "model.hpp"
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

	template<typename Value>
	using back_inserter = std::back_insert_iterator<std::vector<Value>>;

	class loader {
		public:
			loader(nlohmann::json&& json);

			std::unique_ptr<scene>  make_scene();
			std::shared_ptr<camera> make_camera();

		private:
			template<typename OutputIt>
			void init_load(OutputIt out_it);

			nlohmann::json                                     json_;
			std::map<std::string, std::shared_ptr<model_data>> models_{};

			template<typename OutputIt>
			static void load_models_data(const nlohmann::json& models_json, OutputIt out_map);


			void load_models(const nlohmann::json& models_json, back_inserter<std::shared_ptr<model>> inserter);
	};

} // namespace kanso
