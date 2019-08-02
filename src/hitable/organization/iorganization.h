#ifndef I_ORGANIZATION_H
#define I_ORGANIZATION_H

#include <memory>

#include "hitable/ihitable.h"

namespace rt {
	/**
	 * a organization is a hitable that can store other hitables
	 */
	class IOrganization : public IHitable {
	public:
		/**
		 * checks for an intersection between the ray and a hitable in the interval [tmin, tmax]
		 * @param r - ray to test the intersection for
		 * @param tmin - minimal allowed parameter t
		 * @param tmax - maximal allowed parameter t
		 * @param rec - intersection information of the hitable
		 * @return true if the ray intersects the hitable in the given interval, false otherwise
		 */
		virtual bool hit(const ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
		/**
		 * retrieves the axis aligned bounding box of a hitable
		 * @param box - the retrieved bounding box
		 * @return true if the hitable has a bounding box, false otherwise
		 */
		virtual bool boundingbox(aabb& box) const = 0;

		virtual void insert(std::shared_ptr<IHitable> hitable) = 0;
		virtual void insert_all(std::vector<std::shared_ptr<IHitable>> hitables) = 0;
		virtual void build() = 0;
	};
}

#endif//I_ORGANIZATION_H