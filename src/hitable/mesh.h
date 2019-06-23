#ifndef MESH_H
#define MESH_H

#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "hitable.h"
#include "../io/stringutil.h"

struct Triangle {
	vec3 a;
	vec3 b;
	vec3 c;
};

class Mesh : public Hitable {
public:
	Mesh() {}
	Mesh(std::vector<Triangle> triangles, std::shared_ptr<Material> mat) : triangles(triangles), material(mat) {};

	virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const;
	void normalize();

	std::vector<Triangle> triangles;
	std::shared_ptr<Material> material;

private:
	bool hittriangle(const ray& r, const Triangle& tri, float& t) const;
};

bool Mesh::hit(const ray& r, double tMin, double tMax, HitRecord& rec) const {
	float t, tmin = FLT_MAX;
	bool anyhit = false;
	for (const Triangle& tri : triangles) {
		if (hittriangle(r, tri, t) && t < tmin) {
			rec.t = t;
			rec.p = r.position(rec.t);
			rec.normal = cross(normalize(tri.b - tri.a), normalize(tri.c - tri.a));
			rec.material = material;

			tmin = t;
			anyhit = true;
		}
	}

	return anyhit;
};
void Mesh::normalize() {
	// normalize
	vec3 pmin(FLT_MAX, FLT_MAX, FLT_MAX);
	vec3 pmax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (Triangle& tri : triangles) {
		pmin = min(pmin, tri.a);
		pmin = min(pmin, tri.b);
		pmin = min(pmin, tri.c);

		pmax = max(pmax, tri.a);
		pmax = max(pmax, tri.b);
		pmax = max(pmax, tri.c);
	}
	float r = length(pmax - pmin) / 2.f;
	vec3 center = (pmax + pmin) / 2.f;
	for (Triangle& tri : triangles) {
		tri.a = (tri.a - center) / r;
		tri.b = (tri.b - center) / r;
		tri.c = (tri.c - center) / r;
	}
}

bool Mesh::hittriangle(const ray& r, const Triangle& tri, float& t) const {
	// moeller-trumbore algorithm
	vec3 v1 = tri.b - tri.a;
	vec3 v2 = tri.c - tri.a;
	vec3 p = cross(r.dir, v2);
	float det = dot(v1, p);

	// ray and triangle are parallel
	if (fabs(det) < 0.001f) return false;

	float invdet = 1.0f / det;

	vec3 tvec = r.o - tri.a;
	float u = dot(tvec, p) * invdet;
	if (u < 0 || u > 1) return false;

	vec3 qvec = cross(tvec, v1);
	float v = dot(r.dir, qvec) * invdet;
	if (v < 0 || u + v > 1) return false;

	t = dot(v2, qvec) * invdet;

	return true;
}

std::shared_ptr<Mesh> loadmesh(std::string filename, std::shared_ptr<Material> mat) {
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
	std::vector<Triangle> triangles;
	for (size_t i = 2; i < indices.size(); i+=3) {
		Triangle tri;
		tri.a = positions.at(indices.at(i - 2) - 1);
		tri.b = positions.at(indices.at(i - 1) - 1);
		tri.c = positions.at(indices.at(i - 0) - 1);
		triangles.push_back(tri);
	}
	return std::make_shared<Mesh>(triangles, mat);
}

#endif//MESH_H