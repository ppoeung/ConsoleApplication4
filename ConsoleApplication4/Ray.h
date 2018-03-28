#pragma once
#include "Vector.h"

class Ray
{
public:
	Vector C;
	Vector u;

	Ray(Vector C, Vector u) {
		this->C = C;
		this->u = getNormalized(u);
	};

	Ray() { ; };

	~Ray();
	
};

