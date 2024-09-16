#pragma once

#include <glm/vec3.hpp>

#include "core.hpp"

namespace kanso {

	struct light_data {
		glm::vec3 ambient{};
		glm::vec3 diffuse{};
		glm::vec3 specular{};
	};

	class light {
		public:
			light(const light_data& common_part);
			light(light_data&& common_part);
			virtual ~light() = default;

			virtual void load_to(uint shader) = 0;

		protected:
			[[nodiscard]] const light_data& common_data() const {
				return common_part_;
			}

		private:
			light_data common_part_;
	};

	struct point_light_data {
		glm::vec3 pos{};
		float     constant{};
		float     linear{};
		float     quadratic{};
	};

	class point_light : public light {
		public:
			point_light(const light_data& common_part, const point_light_data& point_light_part);
			point_light(light_data&& common_part, point_light_data&& point_light_part);

			void load_to(uint shader) override;

		private:
			point_light_data point_light_part_;
	};

	class directional_light : public light {
		public:
			directional_light(const light_data& common, const glm::vec3& direction);
			directional_light(light_data&& common, glm::vec3&& direction);

			void load_to(uint shader) override;

		private:
			glm::vec3 direction_;
	};

	struct spot_light_data {
		point_light_data point_light_part;
		glm::vec3        direction{};
		float            inner_cut_off{};
		float            outer_cut_off{};
	};

	class spot_light : public light {
		public:
			spot_light(const light_data& common_part, const spot_light_data& spot_light_part);
			spot_light(light_data&& common_part, spot_light_data&& spot_light_part);

			void load_to(uint shader) override;

		private:
			spot_light_data spot_light_part_;
	};

} // namespace kanso
