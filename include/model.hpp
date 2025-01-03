#pragma once

#include <string>

#include "shader.hpp"

#include <glm/vec3.hpp>

namespace kanso {

	class drawable {
		public:
			virtual ~drawable()                                                                          = default;
			virtual void draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) const = 0;
	};

	struct model_view {
		std::string id;
		std::string name;
		std::array<float, 3> pos;
		std::array<float, 3> rot;
		std::array<float, 3> scale;
		std::string type;
	};

	class model : public drawable {
		public:
			model(const shader& render_shader) : render_shader_(render_shader), model_matrix_(1), id_(uuid::generate_id()) {}
			model(std::string_view vert_file, std::string_view frag_file)
				: render_shader_(shader(vert_file, frag_file)),
				  model_matrix_(1) {}

			virtual void select_toggle() = 0;

			uint render_shader() const {
				return render_shader_.id();
			}

			glm::mat4 model_matrix() const {
				return model_matrix_;
			}

			std::string id() const {
				return id_;
			}

			virtual glm::vec3   pos() const            = 0;
			virtual glm::vec3   rot() const            = 0;
			virtual glm::vec3   scale() const          = 0;
			virtual std::string name() const           = 0;
			virtual std::string type() const           = 0;
			virtual bool        is_scene_model() const = 0;

		protected:
			shader    render_shader_;
			mutable glm::mat4 model_matrix_;
			std::string id_;
	};

	class scene_model : public model {
		public:
			scene_model(const shader& render_shader, const shader& outline_shader, const glm::vec3& pos,
			            const glm::vec3& scale, const glm::vec3& rot, const glm::vec3& aabb_min,
			            const glm::vec3& aabb_max)
			    : model(render_shader),
			      outline_shader_(outline_shader),
			      position_(pos),
			      scale_(scale),
			      rotation_(rot),
			      aabb_min_(aabb_min),
			      aabb_max_(aabb_max) {}

			virtual glm::vec3 aabb_min() const = 0;
			virtual glm::vec3 aabb_max() const = 0;

			void select_toggle() override {
				selected_ = !selected_;
			}

			glm::vec3 pos() const override {
				return position_;
			}
			glm::vec3 rot() const override {
				return rotation_;
			}
			glm::vec3 scale() const override {
				return scale_;
			}
			std::string type() const override {
				return "scene_model";
			}
			bool is_scene_model() const override {
				return true;
			}

		protected:
			shader    outline_shader_;
			glm::vec3 position_;
			glm::vec3 scale_;
			glm::vec3 rotation_;
			bool      selected_ = false;
			glm::vec3 aabb_min_;
			glm::vec3 aabb_max_;
	};

} // namespace kanso
