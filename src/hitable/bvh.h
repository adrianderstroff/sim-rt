#ifndef BVH_H
#define BVH_H

#include <vector>

#include "hitable/hitable.h"

namespace rt {
class BVH : public IHitable {
public:
	struct Node {
		std::shared_ptr<Node> left;
		std::shared_ptr<Node> right;
		std::vector<std::shared_ptr<IHitable>> data;
		aabb bounds;
	};

	BVH(size_t maxleafsize = 10, size_t maxrecursiondepth = 50) 
		: m_maxleafsize(maxleafsize), m_maxrecursiondepth(maxrecursiondepth) { }

	void build(std::vector<std::shared_ptr<IHitable>> hitables);

	virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const;
	virtual bool boundingbox(aabb& box) const;

private:
	size_t m_maxleafsize, m_maxrecursiondepth;
	std::shared_ptr<Node> m_root;

	std::shared_ptr<Node> create_node(std::vector<std::shared_ptr<IHitable>> hitables, size_t depth);
	bool hit_node(const ray& r, const std::shared_ptr<Node> node, double tMin, double tMax, HitRecord& rec) const;
};
}

#endif//BVH_H