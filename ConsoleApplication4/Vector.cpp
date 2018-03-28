#include "Vector.h"

#include <random>
static std::default_random_engine engine;
static std::uniform_real_distribution<double> u(0, 1);

Vector Vector::base1() {
	if (fabs(coord[2]) <= fabs(coord[0]) && fabs(coord[2]) <= fabs(coord[1]))
	{
		return getNormalized(Vector(-coord[1], coord[0], 0));
	}
	else if (fabs(coord[1]) <= fabs(coord[0]) && fabs(coord[1]) <= fabs(coord[2]))
	{
		return getNormalized(Vector(-coord[2], 0, coord[0]));
	}
	else
	{
		return getNormalized(Vector(0, -coord[2], coord[1]));
	}
};

Vector Vector::base2() {
	return getNormalized(prodVect(base1(), *this));
}

Vector operator+(const Vector &v1, const Vector &v2) {
	return Vector(v1.coord[0] + v2.coord[0], v1.coord[1] + v2.coord[1], v1.coord[2] + v2.coord[2]);
}

Vector operator-(const Vector &v1, const Vector &v2) {
	return Vector(v1.coord[0] - v2.coord[0], v1.coord[1] - v2.coord[1], v1.coord[2] - v2.coord[2]);
}

Vector operator*(double k, const Vector &v) {
	return Vector(k*v.coord[0], k*v.coord[1], k*v.coord[2]);
}

Vector operator*(const Vector &v, double k) {
	return Vector(k*v.coord[0], k*v.coord[1], k*v.coord[2]);
}

Vector operator/(const Vector &v, double k) {
	return Vector((1 / k)*v.coord[0], (1 / k)*v.coord[1], (1 / k)*v.coord[2]);
}

double prodSca(const Vector &v1, const Vector &v2) {
	return v1.coord[0] * v2.coord[0] + v1.coord[1] * v2.coord[1] + v1.coord[2] * v2.coord[2];
}

Vector prodVect(const Vector &u, const Vector &v) {
	return Vector(u.coord[1] * v.coord[2] - u.coord[2] * v.coord[1], u.coord[2] * v.coord[0] - u.coord[0] * v.coord[2], u.coord[0] * v.coord[1] - u.coord[1] * v.coord[0]);
}

Vector prodHad(const Vector &u, const Vector &v) {
	return Vector(u.coord[0] * v.coord[0], u.coord[1] * v.coord[1], u.coord[2] * v.coord[2]);
}

Vector getNormalized(Vector v) {
	v.normalize();
	return v;
}

Vector dirAlea(Vector N) {
	N.normalize();
	double r1 = u(engine);
	double r2 = u(engine);
	double x = cos(2 * 3.1415926*r1)*sqrt(1 - r2);
	double y = sin(2 * 3.1415926*r1)*sqrt(1 - r2);
	double z = sqrt(r2);
	Vector tan1 = getNormalized(N.base1());
	Vector tan2 = getNormalized(N.base2());
	return x*tan1 + y*tan2 + z*N;
}

Vector reflected(const Vector &inc, const Vector &N) {
	return inc - 2 * prodSca(inc, N)*N;
};
