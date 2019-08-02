#ifndef HITABLE_LIST_H
#define HITABLE_LIST_H

#include <vector>

#include "iorganization.h"

namespace rt {
class HitableList: public IOrganization {
public:
    HitableList() {}

    void insert(std::shared_ptr<IHitable> h) override { m_list.push_back(h); }
	void insert_all(std::vector<std::shared_ptr<IHitable>> hitables) override { m_list.insert(m_list.end(), hitables.begin(), hitables.end()); };
	void build() override { };

    virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const;
	virtual bool boundingbox(aabb& box) const;

private:
    std::vector<std::shared_ptr<IHitable>> m_list;
};
}

#endif//HITABLE_LIST_H