#pragma once

#include "Vector.h"
#include "Ray.h"
#include <math.h>
#include "Object.h"

class Triangle : public Object
{
public:
	Vector A;
	Vector B;
	Vector C;
	Vector N;

	Triangle(const Vector& A, const Vector& B, const Vector& C, Vector rho, bool isMirror = false, bool isLight = false);

	bool intersect(const Ray &ray, Vector &P, Vector &N, double &t, Vector &color) const;

	bool intersect(const Ray &ray, Vector &P, Vector &N, double &t, double &alpha, double &beta, double &gamma) const;


	Vector center() {
		return (1 / 3)*Vector(A[0] + B[0] + C[0], A[2] + B[0] + C[0], A[0] + B[0] + C[0]);
	}

	Triangle();
	~Triangle();
};

