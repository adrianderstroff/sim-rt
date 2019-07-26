#include "mesh.h"

bool rt::Mesh::hit(const ray& r, double tmin, double tmax, HitRecord& rec) const {
	if (bvh.hit(r, tmin, tmax, rec)) {
		// mesh is already in local coordiantes
		// thus the intersection point is in
		// local coordinates as well
		rec.lp = rec.p;

		return true;
	}

	return false;
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

std::shared_ptr<rt::Mesh> rt::load_mesh(std::string filename, std::shared_ptr<IMaterial> mat, bool fliptriangle, bool normalize, bool smoothnormals) {
	console::println("loading file " + filename);
	
	// open file stream
	std::ifstream file(filename);
	if (file.fail()) {
		std::cerr << filename << " does not exist!" << std::endl;
		return nullptr;
	}

	// line for reading
	std::string line;

	// count lines
	size_t numberoflines = 0;
	while (std::getline(file, line))
		++numberoflines;

	// rewind stream
	file.clear();
	file.seekg(0);

	std::vector<vec3> positions, normals, texturecoordinates;
	std::vector<size_t> indices;
	size_t currentlinecount = 0;
	while (std::getline(file, line)) {
		// current progress
		console::progress("loading file       ", static_cast<double>(currentlinecount) / (numberoflines-1));
		++currentlinecount;

		// remove whitespaces at the beginning and end
		line = trim(line);
		if (line.empty()) continue;

		std::vector<std::string> tokens = split(line, ' ');

		if (tokens.at(0) == "v") {
			float x = stof(tokens.at(1));
			float y = stof(tokens.at(2));
			float z = stof(tokens.at(3));
			positions.push_back(vec3(x, y, z));
		}
		/*
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
		*/
		else if (tokens.at(0) == "f") {
			for (size_t i = 1; i < tokens.size(); ++i) {
				std::vector<std::string> vindices = split(tokens.at(i), '/');

				// we are only interested in the first index
				std::string vindex = vindices.at(0);

				// parse to long and process in case its negative
				long idx = (vindex.empty()) ? 1 : stol(vindex);
				while (idx <= 0) { idx = static_cast<long>(positions.size()) + idx; }
				if (idx > positions.size() || idx < 0) console::println("Index out of bounds " + std::to_string(idx) + " " + std::to_string(positions.size()));
				indices.push_back(idx);
			}
		}
	}

	// calculate normals
	if (smoothnormals) calculate_smooth_normals(fliptriangle, indices, positions, normals);
	else               calculate_normals(fliptriangle, indices, positions, normals);
	
	// define offsets
	vec3 offset = (fliptriangle) ? vec3(0, 1, 2) : vec3(2, 1, 0);

	// create triangles
	std::vector<std::shared_ptr<Triangle>> triangles;
	for (size_t i = 2; i < indices.size(); i += 3) {
		console::progress("creating geometry  ", static_cast<double>(i) / (indices.size() - 1));
		size_t idx1 = indices.at(i - static_cast<size_t>(offset.x)) - 1;
		size_t idx2 = indices.at(i - static_cast<size_t>(offset.y)) - 1;
		size_t idx3 = indices.at(i - static_cast<size_t>(offset.z)) - 1;
		vec3 a = positions.at(idx1);
		vec3 b = positions.at(idx2);
		vec3 c = positions.at(idx3);
		vec3 n1 = normals.at(idx1);
		vec3 n2 = normals.at(idx2);
		vec3 n3 = normals.at(idx3);
		triangles.push_back(std::make_shared<Triangle>(a, b, c, n1, n2, n3, mat));
	}

	// create mesh and maybe normalize
	auto mesh = std::make_shared<Mesh>(triangles, mat);
	if (normalize) mesh->normalize();
	
	return mesh;
}

void rt::calculate_normals(bool fliptriangle, const std::vector<size_t>& indices, const std::vector<vec3>& positions, std::vector<vec3>& normals) {
	// make normals fit positions
	normals.resize(positions.size());
	
	// define offsets
	vec3 offset = (fliptriangle) ? vec3(0, 1, 2) : vec3(2, 1, 0);

	// create normals
	for (size_t i = 2; i < indices.size(); i += 3) {
		console::progress("calculating normals", static_cast<double>(i) / (indices.size() - 1));
		size_t idx1 = indices.at(i - static_cast<size_t>(offset.x)) - 1;
		size_t idx2 = indices.at(i - static_cast<size_t>(offset.y)) - 1;
		size_t idx3 = indices.at(i - static_cast<size_t>(offset.z)) - 1;
		vec3 a = positions.at(idx1);
		vec3 b = positions.at(idx2);
		vec3 c = positions.at(idx3);
		vec3 n = rt::normal(a, b, c);
		normals.at(idx1) = n;
		normals.at(idx2) = n;
		normals.at(idx3) = n;
	}
}
void rt::calculate_smooth_normals(bool fliptriangle, const std::vector<size_t>& indices, const std::vector<vec3>& positions, std::vector<vec3>& normals) {
	// make normals fit positions
	normals.resize(positions.size());
	
	// setup temp variables
	std::vector<vec3> tempnormals;
	tempnormals.resize(positions.size());
	std::vector<size_t> normalcounts;
	normalcounts.resize(positions.size());

	// initialize temp variables
	for (size_t i = 0; i < positions.size(); ++i) { 
		tempnormals.at(i) = vec3(0); 
		normalcounts.at(i) = 0;
	}

	// define offsets
	vec3 offset = (fliptriangle) ? vec3(0, 1, 2) : vec3(2, 1, 0);

	// collect normals
	for (size_t i = 2; i < indices.size(); i += 3) {
		console::progress("calculating normals", static_cast<double>(i) / (indices.size() - 1));
		size_t idx1 = indices.at(i - static_cast<size_t>(offset.x)) - 1;
		size_t idx2 = indices.at(i - static_cast<size_t>(offset.y)) - 1;
		size_t idx3 = indices.at(i - static_cast<size_t>(offset.z)) - 1;
		vec3 a = positions.at(idx1);
		vec3 b = positions.at(idx2);
		vec3 c = positions.at(idx3);
		vec3 n = rt::normal(a, b, c);
		tempnormals.at(idx1) += n;
		tempnormals.at(idx2) += n;
		tempnormals.at(idx3) += n;
		normalcounts.at(idx1) += 1;
		normalcounts.at(idx2) += 1;
		normalcounts.at(idx3) += 1;
	}

	// create smoothed normals
	for (size_t i = 2; i < indices.size(); i += 3) {
		size_t idx1 = indices.at(i - offset.x) - 1;
		size_t idx2 = indices.at(i - offset.y) - 1;
		size_t idx3 = indices.at(i - offset.z) - 1;
		vec3 a = positions.at(idx1);
		vec3 b = positions.at(idx2);
		vec3 c = positions.at(idx3);
		normals.at(idx1) = rt::normalize(tempnormals.at(idx1) / normalcounts.at(idx1));
		normals.at(idx2) = rt::normalize(tempnormals.at(idx2) / normalcounts.at(idx2));
		normals.at(idx3) = rt::normalize(tempnormals.at(idx3) / normalcounts.at(idx3));
	}
}