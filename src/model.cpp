#include "model.hpp"
#include "thread_pool.hpp"
#include "shader.hpp"

#include "glad/glad.h"
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/material.h>
#include <assimp/material.inl>
#include <assimp/mesh.h>
#include <assimp/types.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <spdlog/spdlog.h>

#include <stack>
#include <map>

namespace kanso {

	namespace {

		struct tex_data {
				uint8_t*    bytes;
				int32_t     width;
				int32_t     height;
				int32_t     nr_channels;
				std::string path;
				std::string type;
		};

		void collect_ai_meshes_data(aiNode* root_node, const aiScene* scene, std::vector<aiMesh*>& meshes) {
			if (root_node == nullptr || scene == nullptr) {
				return;
			}

			std::stack<aiNode*> node_stack;
			node_stack.push(root_node);

			while (!node_stack.empty()) {
				aiNode* current_node = node_stack.top();
				node_stack.pop();

				for (size_t i = 0; i < current_node->mNumMeshes; i++) {
					aiMesh* mesh =
					    scene->mMeshes[current_node
					                       ->mMeshes[i]]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
					meshes.push_back(mesh);
				}

				for (size_t i = 0; i < current_node->mNumChildren; i++) {
					node_stack.push(
					    current_node->mChildren[i]); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
				}
			}
		}
	} // anonymous namespace

	model_data::model_data(std::string path) : model_name_(std::move(path)) {
		load();
	}

	void model_data::load() {
		load(model_name_);
	}

	void model_data::load(const std::string& path) {
		spdlog::debug("Loading {} model", path);

		Assimp::Importer importer;
		const aiScene*   scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs |
		                                                             aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);
		if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0 ||
		    scene->mRootNode == nullptr) { // NOLINT(hicpp-signed-bitwise)
			throw exception::model_load_exception("Failed to open file " + path);
		}

		std::vector<aiMesh*> ai_meshes;
		collect_ai_meshes_data(scene->mRootNode, scene, ai_meshes);

		auto dir = path.substr(0, path.find_last_of('/'));
		for (const auto& ai_mesh : ai_meshes) {
			meshes_.emplace_back(dir, ai_mesh, scene);
		}
	}

	loaded_model::loaded_model(const shader& render_shader, const shader& outline_shader, const glm::vec3& pos,
	                           const glm::vec3& scale, const glm::vec3& rot, std::shared_ptr<model_data> data)
	    : scene_model(render_shader, outline_shader, pos, scale, rot),
	      data_(std::move(data)) {}

	void loaded_model::draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camera_pos) {
		glStencilFunc(GL_ALWAYS, 1, 0xff);
		glStencilMask(0xff);
		draw_model(get_render_shader(), view, proj, camera_pos);

		if (is_selected()) {
			glStencilFunc(GL_NOTEQUAL, 1, 0xff);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);
			draw_model(get_outline_shader(), view, proj, camera_pos);
			glStencilMask(0xff);
			glStencilFunc(GL_ALWAYS, 0, 0xff);
			glEnable(GL_DEPTH_TEST);
		}
	}

	void loaded_model::draw_model(uint shader, const glm::mat4& view, const glm::mat4& proj,
	                              const glm::vec3& camera_pos) {
		glUseProgram(shader);

		glm::mat4 model{ 1 };
		model = glm::translate(model, get_pos());
		model = glm::scale(model, get_scale());

		model = glm::rotate(model, glm::radians(get_rot()[0]), { 1, 0, 0 });
		model = glm::rotate(model, glm::radians(get_rot()[1]), { 0, 1, 0 });
		model = glm::rotate(model, glm::radians(get_rot()[2]), { 0, 0, 1 });

		shader::set_uniform(shader, "model", model);
		shader::set_uniform(shader, "view", view);
		shader::set_uniform(shader, "proj", proj);

		shader::set_uniform(shader, "viewPos", camera_pos);
		shader::set_uniform(shader, "material.shininess", 32.0f);

		for (auto it = data_->meshes_begin(), end = data_->meshes_end(); it != end; ++it) {
			it->draw(shader);
		}
	}

} // namespace kanso
