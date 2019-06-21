#ifndef HITABLE_LIST_H
#define HITABLE_LIST_H

#include <vector>

#include "hitable.h"

class HitableList: public Hitable {
public:
    HitableList() {}

    void add(std::shared_ptr<Hitable> h) { m_list.push_back(h); }
    virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const;

private:
    std::vector<std::shared_ptr<Hitable>> m_list;
};

bool HitableList::hit(const ray& r, double tMin, double tMax, HitRecord& rec) const {
    HitRecord tempRec;
    
    bool hitAnything = false;
    double closestSoFar = tMax;
    for(size_t i = 0; i < m_list.size(); i++) {
        if(m_list.at(i)->hit(r, tMin, closestSoFar, tempRec)) {
            hitAnything = true;
            closestSoFar = tempRec.t;
            rec = tempRec;
        }
    }

    return hitAnything;
}

#endif//HITABLE_LIST_H