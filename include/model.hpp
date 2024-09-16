#pragma once

#include <string>

#include "drawable.hpp"
#include "mesh.hpp"
#include "exception.hpp"
#include "shader.hpp"

#include <glm/vec3.hpp>

namespace kanso {

	namespace exception {

		class model_load_exception : public base_kanso_exception {
			public:
				model_load_exception(std::string&& msg) : base_kanso_exception(std::move(msg)) {}
		};

	} // namespace exception

	class scene_model : public drawable {
		public:
			scene_model(const shader& render_shader, const shader& outline_shader, const glm::vec3& pos,
			            const glm::vec3& scale, const glm::vec3& rot)
			    : render_shader_(render_shader),
			      outline_shader_(outline_shader),
			      position_(pos),
			      scale_(scale),
			      rotation_(rot) {};

			void draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) override = 0;

			[[nodiscard]] uint get_render_shader() const {
				return render_shader_.get_id();
			}

			[[nodiscard]] uint get_outline_shader() const {
				return outline_shader_.get_id();
			}

			[[nodiscard]] bool is_selected() const {
				return selected_;
			}

			[[nodiscard]] glm::vec3 get_pos() const {
				return position_;
			}

			[[nodiscard]] glm::vec3 get_scale() const {
				return scale_;
			}

			[[nodiscard]] glm::vec3 get_rot() const {
				return rotation_;
			}

			void set_render_shader(const shader& shader) {
				render_shader_ = shader;
			}

			void set_outline_shader(const shader& shader) {
				outline_shader_ = shader;
			}

		private:
			shader    render_shader_;
			shader    outline_shader_;
			glm::vec3 position_;
			glm::vec3 scale_;
			glm::vec3 rotation_;
			bool      selected_ = false;
	};

	class model_data {
		public:
			model_data(std::string path);

			std::vector<mesh>::iterator meshes_begin() {
				return meshes_.begin();
			}

			std::vector<mesh>::iterator meshes_end() {
				return meshes_.end();
			}

		private:
			void load(const std::string& path);
			void load();

			std::vector<mesh> meshes_;
			std::string       model_name_;
	};

	class loaded_model : public scene_model {
		public:
			void draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) override;

			loaded_model(const shader& render_shader, const shader& outline_shader, const glm::vec3& pos,
			             const glm::vec3& scale, const glm::vec3& rot, std::shared_ptr<model_data> data);

		private:
			std::shared_ptr<model_data> data_;

			void draw_model(uint shader, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos);
	};

} // namespace kanso
