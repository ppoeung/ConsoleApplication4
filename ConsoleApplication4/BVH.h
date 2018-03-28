#pragma once
#include "BoundingBox.h"

class BVH
{
public:
	BVH *fg;
	BVH *fd;
	int i0, i1;
	BoundingBox bbox;

	BVH();

};

