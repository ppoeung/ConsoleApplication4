
#include "Sphere.h"



Sphere::Sphere(Vector O, double r, Vector rho, bool isMirror, bool isTransparent, bool isLight) {
	this->O = O;
	this->r = r;
	this->rho = rho;
	this->isMirror = isMirror;
	this->isTransparent = isTransparent;
	this->isLight = isLight;
};

bool Sphere::intersect(const Ray &ray, Vector &P, Vector &N, double &t, Vector &color) const {
	color = rho;
	double a = ray.u.getNorm2(); //est égal à 1 normalement
	double b = 2 * prodSca(ray.u, ray.C - O);
	double c = (ray.C - O).getNorm2() - r*r;
	double delta = b*b - 4 * a*c;
	if (delta < 0)
		return false;
	double t1 = (-b - sqrt(delta)) / (2 * a);
	double t2 = (-b + sqrt(delta)) / (2 * a);
	if (t1 > 0)
		t = t1;
	else if (t2 > 0)
		t = t2;
	else return false;
	P = ray.C + t*ray.u;
	N = getNormalized(P - O);
	return true;
};


Sphere::~Sphere()
{
}
