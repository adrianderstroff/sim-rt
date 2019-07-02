#include "mesh.h"

bool rt::Mesh::hit(const ray& r, double tMin, double tMax, HitRecord& rec) const {
	return bvh.hit(r, tMin, tMax, rec);
};
bool rt::Mesh::boundingbox(aabb& box) const {
	return bvh.boundingbox(box);
}

void rt::Mesh::normalize() {
	// normalize triangles
	aabb bounds;
	if (bvh.boundingbox(bounds)) {
		float r = length(bounds.max() - bounds.min()) / 2.f;
		vec3 center = bounds.center();
		for (auto tri : triangles) {
			tri->p1 = (tri->p1 - center) / r;
			tri->p2 = (tri->p2 - center) / r;
			tri->p3 = (tri->p3 - center) / r;
		}
	}

	// recompute bvh
	std::vector<std::shared_ptr<IHitable>> hitables(triangles.begin(), triangles.end());
	bvh.insert_all(hitables);
	bvh.build();
}

std::shared_ptr<rt::Mesh> rt::loadmesh(std::string filename, std::shared_ptr<IMaterial> mat, bool fliptriangle, bool normalize) {
	std::ifstream file(filename);
	if (file.fail()) {
		std::cerr << filename << " does not exist!" << std::endl;
		return nullptr;
	}

	std::string line;
	std::vector<vec3> positions, normals, texturecoordinates;
	std::vector<size_t> indices;
	while (std::getline(file, line)) {

		line = trim(line);
		if (line.empty()) continue;

		std::vector<std::string> tokens = split(line, ' ');

		if (tokens.at(0) == "v") {
			float x = stof(tokens.at(1));
			float y = stof(tokens.at(2));
			float z = stof(tokens.at(3));
			positions.push_back(vec3(x, y, z));
		}
		else if (tokens.at(0) == "vn") {
			float x = stof(tokens.at(1));
			float y = stof(tokens.at(2));
			float z = stof(tokens.at(3));
			normals.push_back(vec3(x, y, z));
		}
		else if (tokens.at(0) == "vt") {
			float x = stof(tokens.at(1));
			float y = stof(tokens.at(2));
			float z = stof(tokens.at(3));
			texturecoordinates.push_back(vec3(x, y, z));
		}
		else if (tokens.at(0) == "f") {
			for (size_t i = 1; i < tokens.size(); ++i) {
				std::vector<std::string> vindices = split(tokens.at(i), '/');
				for (std::string vindex : vindices) {
					indices.push_back(stoi(vindex));
				}
			}
		}
	}

	// create triangles
	std::vector<std::shared_ptr<Triangle>> triangles;
	if (fliptriangle) {
		for (size_t i = 2; i < indices.size(); i += 3) {
			vec3 a = positions.at(indices.at(i - 0) - 1);
			vec3 b = positions.at(indices.at(i - 1) - 1);
			vec3 c = positions.at(indices.at(i - 2) - 1);
			vec3 n = rt::normal(a, b, c);
			triangles.push_back(std::make_shared<Triangle>(a, b, c, n, n, n, mat));
		}
	}
	else {
		for (size_t i = 2; i < indices.size(); i += 3) {
			Triangle tri;
			vec3 a = positions.at(indices.at(i - 2) - 1);
			vec3 b = positions.at(indices.at(i - 1) - 1);
			vec3 c = positions.at(indices.at(i - 0) - 1);
			vec3 n = rt::normal(a, b, c);
			triangles.push_back(std::make_shared<Triangle>(a, b, c, n, n, n, mat));
		}
	}

	auto mesh = std::make_shared<Mesh>(triangles, mat);
	if (normalize) mesh->normalize();
	
	return mesh;
}