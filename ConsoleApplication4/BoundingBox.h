#pragma once
#include "Vector.h"
#include "Ray.h"
#include <algorithm>

class BoundingBox
{


public:

	Vector minBB, maxBB;

	BoundingBox(){}

	BoundingBox(const Vector& mini, const Vector& maxi);

	bool intersect(const Ray& ray) const;

	~BoundingBox() {};
};

