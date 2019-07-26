#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <fstream>
#include <string>
#include <string>
#include <vector>

#include "io/console.h"
#include "hitable/organization/bvh.h"
#include "hitable/ihitable.h"
#include "math/constants.h"
#include "math/vec3.h"
#include "util/string.h"
#include "triangle.h"

namespace rt {
	class Mesh : public IHitable {
	public:
		Mesh() {}
		Mesh(std::vector<std::shared_ptr<Triangle>> triangles, std::shared_ptr<IMaterial> mat)
			: triangles(triangles), material(mat) {
			std::vector<std::shared_ptr<IHitable>> hitables(triangles.begin(), triangles.end());
			bvh.insert_all(hitables);
			bvh.build();
		};

		virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const override;
		virtual bool boundingbox(aabb& box) const override;

		void normalize();

		BVH bvh;
		std::vector<std::shared_ptr<Triangle>> triangles;
		std::shared_ptr<IMaterial> material;
	};

	std::shared_ptr<Mesh> load_mesh(std::string filename, std::shared_ptr<IMaterial> mat, bool fliptriangle = false, bool normalize = false, bool smoothnormals = false);
	void calculate_normals(bool fliptriangle, const std::vector<size_t>& indices, const std::vector<vec3>& positions, std::vector<vec3>& normals);
	void calculate_smooth_normals(bool fliptriangle, const std::vector<size_t>& indices, const std::vector<vec3>& positions, std::vector<vec3>& normals);
}

#endif//MESH_H