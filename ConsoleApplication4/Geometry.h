#pragma once
#pragma warning(disable:4996)
#include <vector>
#include <list>
#include <string>
#include <map>
#include <iostream>

#include "Vector.h"
#include "Object.h"
#include "Triangle.h"
#include "BoundingBox.h"
#include "BVH.h"

using namespace std;

class TriangleIndices {
public:
	TriangleIndices(int vtxi = -1, int vtxj = -1, int vtxk = -1, int ni = -1, int nj = -1, int nk = -1, int uvi = -1, int uvj = -1, int uvk = -1) : vtxi(vtxi), vtxj(vtxj), vtxk(vtxk), uvi(uvi), uvj(uvj), uvk(uvk), ni(ni), nj(nj), nk(nk) {
	};
	int vtxi, vtxj, vtxk;
	int uvi, uvj, uvk;
	int ni, nj, nk;
	int faceGroup;
};

class Geometry : public Object {
public:
	BoundingBox bb;
	Vector rho;
	bool isMirror;
	bool isTransparent;

	Geometry() {};
	
	Geometry(const char* obj, double scaling, const Vector& offset, Vector rho, bool isMirror = false, bool isTransparent = false);

	BoundingBox buildBBox(int i0, int i1);

	void buildBVH(BVH* node, int i0, int i1);

	bool intersect(const Ray &ray, Vector &P, Vector &N, double &tt, Vector &color) const ;
	
	void readOBJ(const char* obj);

	void add_texture(const char* filename);

	std::vector<TriangleIndices> indices;
	std::vector<Vector> vertices;
	std::vector<Vector> normals;
	std::vector<Vector> uvs; // Vector en 3D mais on n'utilise que 2 composantes
	std::vector<Vector> vertexcolors;

	std::vector<std::vector<unsigned char> > textures;
	std::vector<int> w, h;



private:
	BVH bvh;
};