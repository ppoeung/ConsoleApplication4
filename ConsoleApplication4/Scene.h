#pragma once

#include <vector>
#include <algorithm>    // std::max
#include <iostream>


#include <math.h>
#include "Sphere.h"
#include "Vector.h"
#include "Ray.h"


using namespace std;


class Scene
{
public:
	std::vector<const Object*> objects;
	Vector sourceLum;
	double I;

	Scene(Vector sourceLum, double I);

	Scene() {}

	void addObject(const Object& s) { objects.push_back(&s); }

	bool intersect(const Ray &ray, Vector &P, Vector &N, int &numSphere,double &tt,Vector &color);

	Vector getColor(Ray ray, int nbRec);

	~Scene();

};

