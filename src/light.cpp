#include "light.hpp"
#include "shader.hpp"

namespace kanso {

	light::light(const light_data& common_part) : common_part_(common_part) {}

	light::light(light_data&& common_part) : common_part_(common_part) {}

	point_light::point_light(const light_data& common_part, const point_light_data& point_light)
	    : light(common_part),
	      point_light_part_(point_light) {}

	point_light::point_light(light_data&& common_part, point_light_data&& point_light)
	    : light(common_part),
	      point_light_part_(point_light) {}

	void point_light::bind_to(uint shader) {
		shader::use(shader);

		shader::set_uniform(shader, "pointLight.ambient", common_data().ambient);
		shader::set_uniform(shader, "pointLight.diffuse", common_data().diffuse);
		shader::set_uniform(shader, "pointLight.specular", common_data().specular);

		shader::set_uniform(shader, "pointLight.pos", point_light_part_.pos);
		shader::set_uniform(shader, "pointLight.constant", point_light_part_.constant);
		shader::set_uniform(shader, "pointLight.linear", point_light_part_.linear);
		shader::set_uniform(shader, "pointLight.quadratic", point_light_part_.quadratic);
	}

	directional_light::directional_light(const light_data& common_part, const glm::vec3& direction)
	    : light(common_part),
	      direction_(direction) {}

	directional_light::directional_light(light_data&& common_part, glm::vec3&& direction)
	    : light(common_part),
	      direction_(direction) {}

	void directional_light::bind_to(uint shader) {
		shader::use(shader);

		shader::set_uniform(shader, "dirLight.ambient", common_data().ambient);
		shader::set_uniform(shader, "dirLight.diffuse", common_data().diffuse);
		shader::set_uniform(shader, "dirLight.specular", common_data().specular);

		shader::set_uniform(shader, "dirLight.direction", direction_);
	}

	spot_light::spot_light(const light_data& common_part, const spot_light_data& spot_light_part)
	    : light(common_part),
	      spot_light_part_(spot_light_part) {}

	spot_light::spot_light(light_data&& common_part, spot_light_data&& spot_light_part)
	    : light(common_part),
	      spot_light_part_(spot_light_part) {}

	void spot_light::bind_to(uint shader) {
		shader::use(shader);
		shader::set_uniform(shader, "spotLight.ambient", common_data().ambient);
		shader::set_uniform(shader, "spotLight.diffuse", common_data().diffuse);
		shader::set_uniform(shader, "spotLight.specular", common_data().specular);

		shader::set_uniform(shader, "spotLight.direction", spot_light_part_.direction);

		shader::set_uniform(shader, "spotLight.pos", spot_light_part_.point_light_part.pos);
		shader::set_uniform(shader, "spotLight.constant", spot_light_part_.point_light_part.constant);
		shader::set_uniform(shader, "spotLight.linear", spot_light_part_.point_light_part.linear);
		shader::set_uniform(shader, "spotLight.quadratic", spot_light_part_.point_light_part.quadratic);
	}
} // namespace kanso
