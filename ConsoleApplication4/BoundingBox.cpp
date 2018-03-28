#include "BoundingBox.h"

BoundingBox::BoundingBox(const Vector& mini, const Vector& maxi) {
	this->minBB = mini;
	this->maxBB = maxi;
}


bool BoundingBox::intersect(const Ray& ray) const {
	double tx1 = (minBB[0] - ray.C[0]) / ray.u[0];
	double tx2 = (maxBB[0] - ray.C[0]) / ray.u[0];
	double txmax = std::max(tx1, tx2);
	double txmin = std::min(tx1, tx2);

	double ty1 = (minBB[1] - ray.C[1]) / ray.u[1];
	double ty2 = (maxBB[1] - ray.C[1]) / ray.u[1];
	double tymax = std::max(ty1, ty2);
	double tymin = std::min(ty1, ty2);

	double tz1 = (minBB[2] - ray.C[2]) / ray.u[2];
	double tz2 = (maxBB[2] - ray.C[2]) / ray.u[2];
	double tzmax = std::max(tz1, tz2);
	double tzmin = std::min(tz1, tz2);

	if (std::max(std::max(txmin, tymin), tzmin) < (std::min(std::min(txmax, tymax), tzmax)) && (txmax > 0) && (tymax > 0) && (tzmax > 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}