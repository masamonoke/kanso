#include "primitive.hpp"

#include "glad/glad.h"

namespace kanso {

	void line::draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) {
		(void) camera_pos;
		model_matrix_ = { 1.0f };
		auto mvp = proj * view * model_matrix_;
		shader::use(get_render_shader());
		shader::set_uniform(get_render_shader(), "MVP", mvp);
		renderer_->draw_line();
	}

	line::line(const glm::vec3& start, const glm::vec3& end, std::string_view vert_file, std::string_view frag_file)
	    : model(start, vert_file, frag_file), renderer_(renderer_factory::make_renderer(start, end)) {}

} // namespace kanso
