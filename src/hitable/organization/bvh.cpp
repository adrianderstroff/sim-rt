#include "bvh.h"

void rt::BVH::insert(std::shared_ptr<IHitable> hitable) {
	m_hitables.push_back(hitable);
}
void rt::BVH::insert_all(std::vector<std::shared_ptr<IHitable>> hitables) {
	m_hitables.insert(m_hitables.end(), hitables.begin(), hitables.end());
}

void rt::BVH::build() {
	m_root = create_node(m_hitables, 0);
}

std::shared_ptr<rt::BVH::Node> rt::BVH::create_node(std::vector<std::shared_ptr<IHitable>> hitables, size_t depth) {
	// get surrounding bounding box
	aabb surroundingbox;
	for (auto h : hitables) {
		aabb box;
		if (h->boundingbox(box)) 
			surroundingbox = surrounding_box(surroundingbox, box);
	}

	// create node
	std::shared_ptr<BVH::Node> node = std::make_shared<BVH::Node>();
	node->bounds = surroundingbox;
	node->left = nullptr;
	node->right = nullptr;

	// number of hitables is more than the number limit
	if (hitables.size() <= m_maxleafsize || depth >= m_maxrecursiondepth) {
		node->data = hitables;
	}
	else {
		vec3 dim = surroundingbox.max() - surroundingbox.min();

		// determine longest axis
		vec3 dx(1, 0, 0); vec3 dy(0, 1, 0); vec3 dz(0, 0, 1);
		vec3 axis = (dim.x > dim.y) ? ((dim.x > dim.z) ? dx : dz) : ((dim.y > dim.z) ? dy : dz);

		// calculate center point along axis
		float center = dot(surroundingbox.center(), axis);

		// partition children based on the axis
		std::vector<std::shared_ptr<IHitable>> leftdata;
		std::vector<std::shared_ptr<IHitable>> rightdata;
		for (auto h : hitables) {
			aabb box;
			if (h->boundingbox(box)) {
				float hcenter = dot(box.center(), axis);
				if (hcenter < center) leftdata.push_back(h);
				else                  rightdata.push_back(h);
			}
		}

		// recursively create the hierarchy
		size_t newdepth = ++depth;
		node->left = create_node(leftdata, newdepth);
		node->right = create_node(rightdata, newdepth);
	}

	return node;
}

bool rt::BVH::hit(const ray& r, double tmin, double tmax, HitRecord& rec) const {
	return hit_node(r, m_root, tmin, tmax, rec);
}
bool rt::BVH::hit_node(const ray& r, const std::shared_ptr<Node> node, double tmin, double tmax, HitRecord& rec) const {
	// only test children if ray intersects bounding box
	if (node->bounds.hit(r, tmin, tmax)) {
		bool anyhit = false;

		// leaf node
		if (node->left == nullptr) {
			// check all children and find closest hit
			HitRecord temprec;
			for (auto h : node->data) {
				if (h->hit(r, tmin, tmax, temprec)) {
					tmax = temprec.t;
					rec = temprec;
					anyhit = true;
				}
			}
		}
		else {
			// collect hit record of left and right children
			HitRecord recordleft, recordright;
			recordleft.t  = FLT_MAX;
			recordright.t = FLT_MAX;

			// check children recursively
			bool hitleft  = hit_node(r, node->left, tmin, tmax, recordleft);
			bool hitright = hit_node(r, node->right, tmin, tmax, recordright);
			anyhit = hitleft || hitright;

			// get the record of the right closest child
			if (hitleft && hitright)  rec = (recordleft.t < recordright.t) ? recordleft : recordright;
			else if (hitleft)         rec = recordleft;
			else if (hitright)        rec = recordright;
		}

		return anyhit;
	}

	return false;
}

bool rt::BVH::boundingbox(aabb& box) const {
	// early return if box hasn't been initialized
	if (m_root == nullptr) return false;

	// get bounds of the root node
	box = m_root->bounds;
	return true;
}