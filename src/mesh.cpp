#include "mesh.hpp"

namespace kanso {

	mesh::mesh(mesh_data data)
	    : vertices_(std::move(data.vertices)),
	      indices_(std::move(data.indices)),
	      texture_(data.raw_maps),
		  renderer_(vertices_, indices_) {}

	void mesh::draw(uint shader) {

		texture_.bind(shader);

		renderer_.draw_triangles();
	}
} // namespace kanso
