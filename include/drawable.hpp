#pragma once

#include "core.hpp"

#include <glm/matrix.hpp>

namespace kanso {

	class drawable { // NOLINT(*special-member-functions)
		public:
			virtual ~drawable()                                                                          = default;
			virtual void draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) = 0;
	};

} // namespace kanso
