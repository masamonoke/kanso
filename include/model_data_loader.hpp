#pragma once

#include <map>
#include <mutex>

#include "mesh.hpp"
#include "exception.hpp"

namespace kanso {
	namespace exception {

		class model_load_exception : public base_kanso_exception {
			public:
				model_load_exception(std::string&& msg) : base_kanso_exception(std::move(msg)) {}
		};

	} // namespace exception

	// TODO: very strange class
	class model_data {
		public:
			template <typename InputIt>
			model_data(std::string model_name, InputIt begin, InputIt end);

			std::vector<mesh>::iterator meshes_begin() {
				return meshes_.begin();
			}

			std::vector<mesh>::iterator meshes_end() {
				return meshes_.end();
			}

			glm::vec3 aabb_max() const {
				return aabb_max_;
			}

			glm::vec3 aabb_min() const {
				return aabb_min_;
			}

			std::string name() const { return model_name_; }

		private:
			std::vector<mesh> meshes_;
			std::string       model_name_;
			glm::vec3         aabb_max_{ std::numeric_limits<float>::min() };
			glm::vec3         aabb_min_{ std::numeric_limits<float>::max() };
	};

	class model_data_loader {
		public:
			model_data_loader(std::vector<std::string>::iterator paths_begin,
			                  std::vector<std::string>::iterator paths_end);

			template <typename OutputIt>
			void models_data(OutputIt out) const {
				std::for_each(models_data_.begin(), models_data_.end(), [&out](auto& it) { *out++ = it; });
			}

		private:
			using raw_model_data = std::vector<mesh_data>;

			std::vector<std::pair<std::string, raw_model_data>> raw_models_data_;
			std::map<std::string, std::shared_ptr<model_data>>  models_data_;
			std::mutex                                          mut_;

			template <typename InputIt>
			void load(InputIt paths_begin, InputIt paths_end);

			void load_raw_model(std::string_view path);
	};
} // namespace kanso
