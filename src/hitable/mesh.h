#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <fstream>
#include <string>
#include <string>
#include <vector>

#include "bvh.h"
#include "hitable.h"
#include "triangle.h"
#include "io/stringutil.h"
#include "math/constants.h"
#include "math/vec3.h"

namespace rt {
class Mesh : public IHitable {
public:
	Mesh() {}
	Mesh(std::vector<std::shared_ptr<Triangle>> triangles, std::shared_ptr<IMaterial> mat)
		: triangles(triangles), material(mat) {
		std::vector<std::shared_ptr<IHitable>> hitables(triangles.begin(), triangles.end());
		bvh.build(hitables);
	};

	virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const override;
	virtual bool boundingbox(aabb& box) const override;

	void normalize();

	BVH bvh;
	std::vector<std::shared_ptr<Triangle>> triangles;
	std::shared_ptr<IMaterial> material;
};

std::shared_ptr<Mesh> loadmesh(std::string filename, std::shared_ptr<IMaterial> mat, bool fliptriangle = false, bool normalize = false);
}

#endif//MESH_H