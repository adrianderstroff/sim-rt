#include"cube.h"

rt::Cube::Cube(vec3 pos, float width, float height, float depth, std::shared_ptr<IMaterial> mat, bool invert)
	: m_position(pos), m_width(width), m_height(height), m_depth(depth), m_material(mat) {
	create_rectangles(invert);
}

bool rt::Cube::hit(const ray& r, double tmin, double tmax, HitRecord& record) const {
	bool anyhit = m_rectangles->hit(r, tmin, tmax, record);
	if (anyhit) {
		record.material = m_material;
		record.lp = record.p - m_position;
	}
	return anyhit;
}
bool rt::Cube::boundingbox(aabb& box) const {
	aabb tempbox;
	if (m_rectangles->boundingbox(tempbox)) {
		box = tempbox;
		return true;
	}

	return false;
}

void rt::Cube::create_rectangles(bool invert) {
	vec3 dx(m_width / 2.f, 0, 0);
	vec3 dy(0, m_height / 2.f, 0);
	vec3 dz(0, 0, m_depth / 2.f);

	// initialize bvh
	m_rectangles = std::make_shared<BVH>();

	// create rectangles
	if (!invert) {
		m_rectangles->insert(std::make_shared<Rectangle>(m_position + dx, -dz, dy, nullptr));
		m_rectangles->insert(std::make_shared<Rectangle>(m_position - dx,  dz, dy, nullptr));

		m_rectangles->insert(std::make_shared<Rectangle>(m_position + dy, dx, -dz, nullptr));
		m_rectangles->insert(std::make_shared<Rectangle>(m_position - dy, dx,  dz, nullptr));

		m_rectangles->insert(std::make_shared<Rectangle>(m_position + dz,  dx, dy, nullptr));
		m_rectangles->insert(std::make_shared<Rectangle>(m_position - dz, -dx, dy, nullptr));
	}
	else {
		m_rectangles->insert(std::make_shared<Rectangle>(m_position + dx,  dz, dy, nullptr));
		m_rectangles->insert(std::make_shared<Rectangle>(m_position - dx, -dz, dy, nullptr));

		m_rectangles->insert(std::make_shared<Rectangle>(m_position + dy, dx,  dz, nullptr));
		m_rectangles->insert(std::make_shared<Rectangle>(m_position - dy, dx, -dz, nullptr));

		m_rectangles->insert(std::make_shared<Rectangle>(m_position + dz, -dx, dy, nullptr));
		m_rectangles->insert(std::make_shared<Rectangle>(m_position - dz,  dx, dy, nullptr));
	}
	m_rectangles->build();
}