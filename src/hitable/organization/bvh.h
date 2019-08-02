#ifndef BVH_H
#define BVH_H

#include <vector>

#include "iorganization.h"
#include "io/console.h"

namespace rt {
class BVH : public IOrganization {
public:
	struct Node {
		std::shared_ptr<Node> left;
		std::shared_ptr<Node> right;
		std::vector<std::shared_ptr<IHitable>> data;
		aabb bounds;
	};

	BVH(size_t maxleafsize = 10, size_t maxrecursiondepth = 50) 
		: m_maxleafsize(maxleafsize), m_maxrecursiondepth(maxrecursiondepth) { }

	void insert(std::shared_ptr<IHitable> hitable) override;
	void insert_all(std::vector<std::shared_ptr<IHitable>> hitables) override;
	void build() override;

	virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const override;
	virtual bool boundingbox(aabb& box) const override;

private:
	size_t m_maxleafsize, m_maxrecursiondepth;
	std::shared_ptr<Node> m_root;
	std::vector<std::shared_ptr<IHitable>> m_hitables;

	std::shared_ptr<Node> create_node(std::vector<std::shared_ptr<IHitable>> hitables, size_t depth);
	bool hit_node(const ray& r, const std::shared_ptr<Node> node, double tMin, double tMax, HitRecord& rec) const;
};
}

#endif//BVH_H