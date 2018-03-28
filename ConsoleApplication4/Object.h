#pragma once

#include "Vector.h"
#include "Ray.h"

class Object
{
public:
	Object();
	Vector rho;
	bool isMirror;
	bool isLight;
	bool isTransparent;
	virtual bool intersect(const Ray &ray, Vector &P, Vector &N, double &t, Vector &color) const = 0;



};

