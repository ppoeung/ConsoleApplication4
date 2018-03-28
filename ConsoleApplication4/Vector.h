#pragma once

#include <math.h>


class Vector
{
public:
	Vector(double x, double y, double z) {
		coord[0] = x;
		coord[1] = y;
		coord[2] = z;
	}

	Vector() {};
	
	double coord[3];

	double operator[](int i) const {
		return coord[i];
	};

	double& operator[](int i)  {
		return coord[i];
	};

	void operator+=(const Vector &b) {
		coord[0] += b.coord[0];
		coord[1] += b.coord[1];
		coord[2] += b.coord[2];
	}

	double getNorm2() const {
		return coord[0] * coord[0] + coord[1] * coord[1] + coord[2] * coord[2];
	}

	void normalize() {
		double norm = sqrt(getNorm2());
		coord[0] = coord[0] / norm;
		coord[1] = coord[1] / norm;
		coord[2] = coord[2] / norm;
	}




	Vector base1();

	Vector base2();
	


private:

};

 

Vector operator+(const Vector &v1, const Vector &v2);



Vector operator-(const Vector &v1, const Vector &v2);

Vector operator*(double k, const Vector &v);

Vector operator*(const Vector &v, double k);
Vector operator/(const Vector &v, double k);

double prodSca(const Vector &v1, const Vector &v2);

Vector prodVect(const Vector &v1, const Vector &v2);

Vector prodHad(const Vector &v1, const Vector &v2);

Vector getNormalized(Vector v);

Vector dirAlea(Vector N);

Vector reflected(const Vector &inc, const Vector &N);


