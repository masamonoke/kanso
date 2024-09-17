#include "loader.hpp"
#include "model_data_loader.hpp"

#include <memory>
#include <iostream>

#include <spdlog/spdlog.h>

namespace kanso {

	loader::loader(nlohmann::json&& json) : json_(std::move(json)), models_(init_load()) {}

	namespace {
		void load_light(const nlohmann::json& lights_json, std::vector<std::shared_ptr<light>>& lights);
		std::pair<shader, shader> create_shader(const std::string& render_path, const std::string& outline_path);
		glm::vec3                 from_json_to_vec3(const nlohmann::json& j, const std::string& name);
	} // namespace

	std::map<std::string, std::shared_ptr<model_data>> loader::init_load() {
		for (const auto& j : json_) {
			try {
				if (j["type"] == "model") {
					return load_models_data(j);
				}
			} catch (const nlohmann::json::type_error& e) {
				spdlog::error(e.what());
				throw exception::bad_scene_file_exception("Passed scene file is in wrong format");
			}
		}

		return {};
	}

	std::unique_ptr<scene> loader::make_scene() {
		spdlog::debug("Making scene");

		std::vector<std::shared_ptr<light>>        lights;
		std::vector<std::shared_ptr<loaded_model>> models;

		for (const auto& j : json_) {
			try {
				if (j["type"] == "model") {
					load_models(j, models);
				} else if (j["type"] == "light") {
					load_light(j, lights);
				}
			} catch (const nlohmann::json::type_error& e) {
				spdlog::error(e.what());
				throw exception::bad_scene_file_exception("Passed scene file is in wrong format");
			}
		}

		return std::make_unique<scene>(models, lights);
	}

	std::shared_ptr<camera> loader::make_camera() {
		spdlog::debug("Making camera");

		for (const auto& j : json_) {
			try {
				if (j["type"] == "camera") {
					const glm::vec3 pos  = from_json_to_vec3(j, "position");
					const float     fov  = j["fov"];
					const float     near = j["near"];
					const float     far  = j["far"];
					return std::make_shared<camera>(pos, fov, near, far);
				}
			} catch (const nlohmann::json::type_error& e) {
				spdlog::error(e.what());
				throw exception::bad_scene_file_exception("Passed scene file is in wrong format");
			}
		}

		throw exception::camera_not_found_exception("Scene file has no definition for camera");
	}

	std::map<std::string, std::shared_ptr<model_data>> loader::load_models_data(const nlohmann::json& models_json) {
		std::vector<std::string> paths;
		for (const auto& json : models_json["values"]) {
			paths.emplace_back(json["path"]);
		}

		const model_data_loader loader{ paths };
		return loader.get_models_data();
	}

	void loader::load_models(const nlohmann::json&                       models_json,
	                         std::vector<std::shared_ptr<loaded_model>>& loaded_models) {
		for (const auto& model_json : models_json["values"]) {
			try {
				auto      data = models_[model_json["path"]];
				glm::vec3 pos;
				if (model_json.contains("position")) {
					pos = from_json_to_vec3(model_json, "position");
				} else {
					pos = { 0, 0, 0 };
				}
				glm::vec3 scale;
				if (model_json.contains("scale")) {
					scale = from_json_to_vec3(model_json, "scale");
				} else {
					scale = { 1, 1, 1 };
				}
				glm::vec3 rot;
				if (model_json.contains("rotation")) {
					rot = from_json_to_vec3(model_json, "rotation");
				} else {
					rot = { 0, 0, 0 };
				}
				auto shaders = create_shader(model_json["render_shader"], model_json["outline_shader"]);
				loaded_models.emplace_back(
				    std::make_shared<loaded_model>(shaders.first, shaders.second, pos, scale, rot, data));
			} catch (const exception::model_load_exception& e) {
				if (model_json["path"].is_string()) {
					if (model_json["path"].is_string()) {
						spdlog::error("Failed to load model {}", std::string{ model_json["path"] });
					} else {
						spdlog::error("Failed to load model and path is not even string");
					}
				}
				continue;
			}
		}
	}

	namespace {

		light_data make_common_light(const nlohmann::json& light_json) {
			auto ambient  = from_json_to_vec3(light_json, "ambient");
			auto diffuse  = from_json_to_vec3(light_json, "diffuse");
			auto specular = from_json_to_vec3(light_json, "specular");

			return { ambient, diffuse, specular };
		}

		point_light_data make_point_light(const nlohmann::json& light_json) {
			const float constant  = light_json["constant"];
			const float linear    = light_json["linear"];
			const float quadratic = light_json["quadratic"];
			auto        pos       = from_json_to_vec3(light_json, "position");

			return { pos, constant, linear, quadratic };
		}

		spot_light_data make_spot_light(nlohmann::json light_json) {
			const float            inner_cut_off = light_json["inner_cut_off_angle"];
			const float            outer_cut_off = light_json["outer_cut_off_angle"];
			const float            constant      = light_json["constant"];
			const float            linear        = light_json["linear"];
			const float            quadratic     = light_json["quadratic"];
			auto                   direction     = from_json_to_vec3(light_json, "direction");
			auto                   pos           = from_json_to_vec3(light_json, "position");
			const point_light_data point_part{ pos, constant, linear, quadratic };

			return { point_part, direction, inner_cut_off, outer_cut_off };
		}

		void load_light(const nlohmann::json& lights_json, std::vector<std::shared_ptr<light>>& lights) {
			for (const auto& light_json : lights_json["values"]) {
				if (light_json["light_type"] == "POINT_LIGHT") {
					auto light_common = make_common_light(light_json);
					auto point_data   = make_point_light(light_json);
					lights.emplace_back(std::make_shared<point_light>(light_common, point_data));
				} else if (light_json["light_type"] == "SPOT_LIGHT") {
					auto light_common = make_common_light(light_json);
					auto spot_data    = make_spot_light(light_json);
					lights.emplace_back(std::make_shared<spot_light>(light_common, spot_data));
				} else if (light_json["light_type"] == "DIRECTIONAL_LIGHT") {
					auto dir          = from_json_to_vec3(light_json, "direction");
					auto light_common = make_common_light(light_json);
					lights.emplace_back(std::make_shared<directional_light>(light_common, dir));
				} else {
					spdlog::warn("Unknow light type: {}", std::string{ light_json["light_type"] });
				}
			}
		}

		std::pair<shader, shader> create_shader(const std::string& render_path, const std::string& outline_path) {
			// NOTE:
			// There is some bug in clang-tidy that falsely detects "No member named 'format' in namespace 'std'"
			// so i used fmt version from transitive dependency of spdlog that is fmt library.
			// If you use std::format it compiles.
			const auto render_vert = fmt::format("shaders/{}.vert", render_path);
			const auto render_frag = fmt::format("shaders/{}.frag", render_path);

			const auto outline_vert = fmt::format("shaders/{}.vert", outline_path);
			const auto outline_frag = fmt::format("shaders/{}.frag", outline_path);

			return { { render_vert, render_frag }, { outline_vert, outline_frag } };
		}

		glm::vec3 from_json_to_vec3(const nlohmann::json& j, const std::string& name) {
			return { j[name][0], j[name][1], j[name][2] };
		}

	} // anonymous namespace

} // namespace kanso
