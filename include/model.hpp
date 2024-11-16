#pragma once

#include <string>

#include "shader.hpp"

#include <glm/vec3.hpp>

namespace kanso {

	class drawable {
		public:
			virtual ~drawable()                                                                          = default;
			virtual void draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) = 0;
	};

	class model : public drawable {
		public:
			model(const shader& render_shader, const glm::vec3& pos)
			    : render_shader_(render_shader),
			      model_matrix_(1),
			      pos_(pos) {}
			model(const glm::vec3& pos, std::string_view vert_file, std::string_view frag_file)
			    : render_shader_(shader(vert_file, frag_file)),
			      model_matrix_(1),
			      pos_(pos) {}

			virtual void select_toggle() = 0;

			uint render_shader() {
				return render_shader_.id();
			}

			glm::mat4 model_matrix() const {
				return model_matrix_;
			}

			glm::vec3 pos() const {
				return pos_;
			}

		protected:
			shader    render_shader_;
			glm::mat4 model_matrix_;
			glm::vec3 pos_;
	};

	class scene_model : public model {
		public:
			scene_model(const shader& render_shader, const shader& outline_shader, const glm::vec3& pos,
			            const glm::vec3& scale, const glm::vec3& rot, const glm::vec3& aabb_min,
			            const glm::vec3& aabb_max)
			    : model(render_shader, pos),
			      outline_shader_(outline_shader),
			      position_(pos),
			      scale_(scale),
			      rotation_(rot),
			      aabb_min_(aabb_min),
			      aabb_max_(aabb_max) {}

			virtual glm::vec3 aabb_min() = 0;

			virtual glm::vec3 aabb_max() = 0;

			void select_toggle() override {
				selected_ = !selected_;
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
