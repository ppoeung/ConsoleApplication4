#include "Triangle.h"


Triangle::Triangle(const Vector& A, const Vector& B, const Vector& C, Vector rho, bool isMirror, bool isLight) {
	this->A = A;
	this->B = B;
	this->C = C;
	this->N = getNormalized(prodVect(A - B, A - C));
	this->rho = rho;
	this->isMirror = isMirror;
	this->isLight = isLight;
};


bool Triangle::intersect(const Ray &ray, Vector &P, Vector &N, double &t, Vector &color) const {
	double alpha, beta, gamma;
	color = rho;
	return intersect(ray, P, N, t, alpha, beta, gamma);
}

bool Triangle::intersect(const Ray &ray, Vector &P, Vector &N, double &t, double &alpha, double &beta, double &gamma) const {
	N = this->N;
	t = prodSca(A - ray.C, N) / prodSca(ray.u, N);
	if (t < 0) return false;
	P = ray.C + t*ray.u;
	double APAB = prodSca(P - A, B - A);
	double ACAC = (C - A).getNorm2();
	double APAC = prodSca(P - A, C - A);
	double ACAB = prodSca(C - A, B - A);
	double ABAB = (B - A).getNorm2();
	double detM = ABAB*ACAC - ACAB*ACAB;
	beta = (APAB*ACAC - APAC*ACAB) / detM;
	if (beta < 0 || beta > 1) return false;
	gamma = (APAC*ABAB - APAB*ACAB) / detM;
	if (gamma < 0 || gamma > 1) return false;
	alpha = 1 - beta - gamma;
	if (alpha < 0 || alpha > 1) return false;
	N.normalize();
	return true;

};


Triangle::~Triangle()
{
}
