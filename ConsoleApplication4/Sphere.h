#pragma once

#include "Vector.h"
#include "Ray.h"
#include <math.h>
#include "Object.h"

class Sphere: public Object
{
public:
	Vector O;
	double r;

	Sphere() {};

	Sphere(Vector O, double r, Vector rho, bool isMirror = false, bool isTransparent = false, bool isLight = false);

	bool intersect(const Ray &ray, Vector &P, Vector &N, double &t, Vector &color) const;

	~Sphere();
};

