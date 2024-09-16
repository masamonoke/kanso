#pragma once

#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <assimp/mesh.h>
#include <assimp/types.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "texture.hpp"

namespace kanso {

	namespace internal {
		struct vertex_object {
				uint vao;
				uint vbo;
				uint ebo;
		};
	} // namespace interntal

	struct mesh_vertex {
			glm::vec3 pos{};
			glm::vec3 normal{};
			glm::vec2 tex_coords{};
	};

	class mesh {
		public:
			mesh(const std::string& dir, const aiMesh* ai_mesh, const aiScene* scene);
			~mesh();

			void draw(uint shader);

		private:
			std::vector<mesh_vertex> vertices_;
			std::vector<int>         indices_;
			std::vector<texture>     textures_;
			internal::vertex_object vo_{};
	};

} // namespace kanso
