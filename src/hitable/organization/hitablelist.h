#ifndef HITABLE_LIST_H
#define HITABLE_LIST_H

#include <vector>

#include "hitable/ihitable.h"

namespace rt {
class HitableList: public IHitable {
public:
    HitableList() {}

    void insert(std::shared_ptr<IHitable> h) { m_list.push_back(h); }

    virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const;
	virtual bool boundingbox(aabb& box) const;

private:
    std::vector<std::shared_ptr<IHitable>> m_list;
};
}

#endif//HITABLE_LIST_H