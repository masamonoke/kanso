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

	class model_data {
		public:
			model_data(std::string model_name, std::vector<mesh_data> meshes_data);

			std::vector<mesh>::iterator meshes_begin() {
				return meshes_.begin();
			}

			std::vector<mesh>::iterator meshes_end() {
				return meshes_.end();
			}

		private:
			std::vector<mesh> meshes_;
			std::string       model_name_;
	};

	class model_data_loader {
		public:
			model_data_loader(const std::vector<std::string>& paths);

			[[nodiscard]] std::map<std::string, std::shared_ptr<model_data>> get_models_data() const {
				return models_data_;
			}

		private:
			using raw_model_data = std::vector<mesh_data>;

			std::vector<std::pair<std::string, raw_model_data>> raw_models_data_;
			std::map<std::string, std::shared_ptr<model_data>>  models_data_;
			std::mutex                                          mut_;

			void load(const std::vector<std::string>& paths);
			void load_raw_model(const std::string& path);
	};
} // namespace kanso
