#pragma once

#include <string>

#include "core.hpp"
#include "exception.hpp"

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

namespace kanso {

	namespace exception {

		class shader_load_exception : public base_kanso_exception {
			public:
				shader_load_exception(std::string&& msg) : base_kanso_exception(std::move(msg)) {}
		};

		class shader_compile_exception : public base_kanso_exception {
			public:
				shader_compile_exception(std::string&& msg) : base_kanso_exception(std::move(msg)) {}
		};

		class shader_linkage_exception : public base_kanso_exception {
			public:
				shader_linkage_exception(std::string&& msg) : base_kanso_exception(std::move(msg)) {}
		};

	} // namespace exception

	class shader {
		public:
			shader(const std::string& vert_file, const std::string& frag_file);

			[[nodiscard]] uint get_id() const {
				return id_;
			}

			static void set_uniform(uint shader, const std::string& name, const glm::vec3& vector);
			static void set_uniform(uint shader, const std::string& name, const glm::mat4& matrix);
			static void set_uniform(uint shader, const std::string& name, bool val);
			static void set_uniform(uint shader, const std::string& name, int val);
			static void set_uniform(uint shader, const std::string& name, float val);
			static void use(uint shader);

		private:
			uint id_;
	};

} // namespace kanso
