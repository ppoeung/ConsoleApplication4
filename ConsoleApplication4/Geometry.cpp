#include "Geometry.h"



Geometry::Geometry(const char* obj, double scaling, const Vector& offset, Vector rho, bool isMirror, bool isTransparent) {
	readOBJ(obj);
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i] = vertices[i] * scaling + offset;
	}
	Vector minBB = Vector(1E99, 1E99, 1E99);
	Vector maxBB = Vector(-1E99, -1E99, -1E99);
	this->bb = buildBBox(0, vertices.size());
	this->rho = rho;
	this->isMirror = isMirror;
	this->isTransparent = isTransparent;
	buildBVH(&bvh, 0, indices.size());
}

BoundingBox Geometry::buildBBox(int i0, int i1) { //          
	Vector minBB = Vector(1E99, 1E99, 1E99);
	Vector maxBB = Vector(-1E99, -1E99, -1E99);
	for (int i = i0; i < i1; i++) {
		for (int dim = 0; dim < 3; dim++) {
			minBB[dim] = std::min(minBB[dim], vertices[indices[i].vtxi][dim]);
			minBB[dim] = std::min(minBB[dim], vertices[indices[i].vtxj][dim]);
			minBB[dim] = std::min(minBB[dim], vertices[indices[i].vtxk][dim]);
			maxBB[dim] = std::max(maxBB[dim], vertices[indices[i].vtxi][dim]);
			maxBB[dim] = std::max(maxBB[dim], vertices[indices[i].vtxj][dim]);
			maxBB[dim] = std::max(maxBB[dim], vertices[indices[i].vtxk][dim]);
		}
	}
	return BoundingBox(minBB, maxBB);
}

void Geometry::buildBVH(BVH* node, int i0, int i1) {
	node->bbox = buildBBox(i0, i1);
	node->i0 = i0;
	node->i1 = i1;
	node->fg = NULL;
	node->fd = NULL;
	Vector diag = node->bbox.maxBB - node->bbox.minBB;
	int dimSplit = 2;
	if ((diag[0] > diag[1]) && (diag[0] > diag[2])) {
		dimSplit = 0;
	}
	else if ((diag[1] > diag[0]) && (diag[1] > diag[2])) {
		dimSplit = 1;
	}
	double valSplit = node->bbox.minBB[dimSplit] + diag[dimSplit] * 0.5;
	int pivot = i0 - 1;
	for (int i = i0; i < 1; i++) {
		Triangle tri = Triangle(vertices[indices[i].vtxi], vertices[indices[i].vtxj], vertices[indices[i].vtxk], Vector(0.2, 0.2, 0.2));
		double centerDimSplit = tri.center()[dimSplit];
		if (centerDimSplit < valSplit) {
			pivot++;
			std::swap(vertices[indices[i].vtxi], vertices[indices[pivot].vtxi]);
			std::swap(vertices[indices[i].vtxj], vertices[indices[pivot].vtxj]);
			std::swap(vertices[indices[i].vtxk], vertices[indices[pivot].vtxk]);
			std::swap(uvs[indices[i].uvi], vertices[indices[pivot].uvi]);
			std::swap(uvs[indices[i].uvj], vertices[indices[pivot].uvj]);
			std::swap(uvs[indices[i].uvk], vertices[indices[pivot].uvk]);
			std::swap(normals[indices[i].ni], normals[indices[pivot].ni]);
			std::swap(normals[indices[i].nj], normals[indices[pivot].nj]);
			std::swap(normals[indices[i].nk], normals[indices[pivot].nk]);
			std::swap(indices[i].faceGroup, indices[pivot].faceGroup);
		}
	}

	if (pivot <= i0 || pivot >= i1-1 || i1==i0+1) return;

	node->fg = new BVH();
	buildBVH(node->fg, i0, pivot+1);

	node->fd = new BVH();
	buildBVH(node->fg, pivot+1, i1);
};


   //  ################    < v4 avec texture >
bool Geometry::intersect(const Ray &ray, Vector &P, Vector &N, double &tt, Vector &color) const {
	bool has_inter = false;
	double t;
	double tmin = 1E99;
	Vector localP;
	Vector localN;
	int imin;
	double alphamin, betamin, gammamin;

	if (!bvh.bbox.intersect(ray)) return false;

	std::list <const BVH*> liste;
	liste.push_front(&bvh);

	while (!liste.empty()) {
		const BVH* current = liste.front();
		liste.pop_front();
		if (current->fg && current->fg->bbox.intersect(ray)) {
			liste.push_back(current->fg);
		}
		if (current->fd && current->fd->bbox.intersect(ray)) {
			liste.push_back(current->fd);
		}
		if (!current->fg) { //feuille
			for (int i = current->i0; i < current->i1; i++) {
				int a = indices[i].vtxi;
				int b = indices[i].vtxj;
				int c = indices[i].vtxk;
				Triangle tri = Triangle(vertices[a], vertices[b], vertices[c], Vector(0.2, 0.2, 0.2));
				double alpha, beta, gamma;
				bool local_has_inter = tri.intersect(ray, localP, localN, t, alpha, beta, gamma);
				if (local_has_inter)
				{
					has_inter = true;
					if (t < tmin) {
						tmin = t;
						imin = i;
						P = localP;
						alphamin = alpha;
						betamin = beta;
						gammamin = gamma;

					}
				}
			}

		}
	}
	if (has_inter) {
		N = alphamin*normals[indices[imin].ni] + betamin*normals[indices[imin].nj] + gammamin*normals[indices[imin].nk];
		int texID = indices[imin].faceGroup;
		Vector UV = alphamin*uvs[indices[imin].uvi] + betamin*uvs[indices[imin].uvj] + gammamin*uvs[indices[imin].uvk];

		int x = UV[0] * (w[texID] - 1);
		int y = UV[1] * (h[texID] - 1);
		double rr = textures[texID][(y*w[texID] + x) * 3] / 255.;
		double bb = textures[texID][(y*w[texID] + x) * 3 + 1] / 255.;
		double gg = textures[texID][(y*w[texID] + x) * 3 + 2] / 255.;
		color = Vector(rr, bb, gg);
		tt = tmin;
	}
	return has_inter;
}




/*
//  ################    < v3 avec BVH >

bool Geometry::intersect(const Ray &ray, Vector &P, Vector &N, double &tt, Vector &color) const {
	bool has_inter = false;
	double t;
	double tmin = 1E99;
	Vector localP;
	Vector localN;
	Triangle tri;
	color = rho;

	if (!bvh.bbox.intersect(ray)) return false;

	std::list <const BVH*> liste;
	liste.push_front(&bvh);

	while (!liste.empty()) {
		const BVH* current = liste.front();
		liste.pop_front();
		if (current->fg && current->fg->bbox.intersect(ray)) { // existance d'un sous arbre gauche et intersection avec le sous arbre gauche
			liste.push_back(current->fg);
		}
		if (current->fd && current->fd->bbox.intersect(ray)) { // existance d'un sous arbre droit et intersection avec le sous arbre droit
			liste.push_back(current->fd);
		}
		if (!current->fg) { //l'arbre gauche est une feuille
			for (int i = current->i0; i < current->i1; i++) {
				tri = Triangle(vertices[indices[i].vtxi], vertices[indices[i].vtxj], vertices[indices[i].vtxk], Vector(0.1, 0.1, 0.1));
				bool local_has_inter = tri.intersect(ray, localP, localN, t, color);
				if (local_has_inter)
				{
					has_inter = true;
					if (t < tmin) {
						tmin = t;
						P = localP;
						N = localN;
					}
				}
			}
		}
	}
	tt = tmin;
	return has_inter;
}*/

/*
       // ################    < v2 avec boundingbox >
bool  Geometry::intersect(const Ray &ray, Vector &P, Vector &N, double &tt, Vector &color) const {
	if (!bb.intersect(ray)) return false;   //enlever pour la v1
	bool has_inter = false;
	double t;
	double tmin = 1E99;
	Vector localP;
	Vector localN;
	color = Vector(0, 0, 0);
	for (int i = 0; i < indices.size(); i++) {
		Triangle tri = Triangle(vertices[indices[i].vtxi], vertices[indices[i].vtxj], vertices[indices[i].vtxk],Vector(0.2,0.2,0.2));
		bool local_has_inter = tri.intersect(ray, localP, localN, t, color);
		if (local_has_inter)
		{
			has_inter = true;
			if (t < tmin) {
				tmin = t;
				P = localP;
				N = (-1)*localN;
			}
		}
	}
	tt = tmin;
	return has_inter;

}
*/



void Geometry::readOBJ(const char* obj) {

	char matfile[255];
	char grp[255];

	FILE* f;
	f = fopen(obj, "r");

	std::map<std::string, int> groupNames;
	int curGroup = -1;
	while (!feof(f)) {
		char line[255];
		if (!fgets(line, 255, f)) break;

		std::string linetrim(line);
		linetrim.erase(linetrim.find_last_not_of(" \r\t") + 1);
		strcpy(line, linetrim.c_str());

		if (line[0] == 'u' && line[1] == 's') {
			sscanf(line, "usemtl %[^\n]\n", grp);
			if (groupNames.find(std::string(grp)) != groupNames.end()) {
				curGroup = groupNames[std::string(grp)];
			}
			else {
				curGroup = groupNames.size();
				groupNames[std::string(grp)] = curGroup;
			}
		}
		if (line[0] == 'm' && line[1] == 't' && line[2] == 'l') {
			sscanf(line, "mtllib %[^\n]\n", matfile);
		}
		if (line[0] == 'v' && line[1] == ' ') {
			Vector vec;
			Vector col;
			if (sscanf(line, "v %lf %lf %lf %lf %lf %lf\n", &vec[0], &vec[2], &vec[1], &col[0], &col[1], &col[2]) == 6) {
				vertices.push_back(vec);
				vertexcolors.push_back(col);
			}
			else {
				sscanf(line, "v %lf %lf %lf\n", &vec[0], &vec[2], &vec[1]);  // helmet
																			 //vec[2] = -vec[2]; //car2
				vertices.push_back(vec);
			}
		}
		if (line[0] == 'v' && line[1] == 'n') {
			Vector vec;
			sscanf_s(line, "vn %lf %lf %lf\n", &vec[0], &vec[2], &vec[1]); //girl
			normals.push_back(vec);
		}
		if (line[0] == 'v' && line[1] == 't') {
			Vector vec;
			sscanf(line, "vt %lf %lf\n", &vec[0], &vec[1]);
			uvs.push_back(vec);
		}
		if (line[0] == 'f') {
			TriangleIndices t;
			int i0, i1, i2, i3;
			int j0, j1, j2, j3;
			int k0, k1, k2, k3;
			int nn;

			char* consumedline = line + 1;
			int offset;
			t.faceGroup = curGroup;
			nn = sscanf(consumedline, "%u/%u/%u %u/%u/%u %u/%u/%u%n", &i0, &j0, &k0, &i1, &j1, &k1, &i2, &j2, &k2, &offset);
			if (nn == 9) {
				if (i0 < 0) t.vtxi = vertices.size() + i0; else t.vtxi = i0 - 1;
				if (i1 < 0) t.vtxj = vertices.size() + i1; else t.vtxj = i1 - 1;
				if (i2 < 0) t.vtxk = vertices.size() + i2; else t.vtxk = i2 - 1;
				if (j0 < 0) t.uvi = uvs.size() + j0; else   t.uvi = j0 - 1;
				if (j1 < 0) t.uvj = uvs.size() + j1; else   t.uvj = j1 - 1;
				if (j2 < 0) t.uvk = uvs.size() + j2; else   t.uvk = j2 - 1;
				if (k0 < 0) t.ni = normals.size() + k0; else    t.ni = k0 - 1;
				if (k1 < 0) t.nj = normals.size() + k1; else    t.nj = k1 - 1;
				if (k2 < 0) t.nk = normals.size() + k2; else    t.nk = k2 - 1;

				indices.push_back(t);
			}
			else {
				nn = sscanf(consumedline, "%u/%u %u/%u %u/%u%n", &i0, &j0, &i1, &j1, &i2, &j2, &offset);
				if (nn == 6) {
					if (i0 < 0) t.vtxi = vertices.size() + i0; else t.vtxi = i0 - 1;
					if (i1 < 0) t.vtxj = vertices.size() + i1; else t.vtxj = i1 - 1;
					if (i2 < 0) t.vtxk = vertices.size() + i2; else t.vtxk = i2 - 1;
					if (j0 < 0) t.uvi = uvs.size() + j0; else   t.uvi = j0 - 1;
					if (j1 < 0) t.uvj = uvs.size() + j1; else   t.uvj = j1 - 1;
					if (j2 < 0) t.uvk = uvs.size() + j2; else   t.uvk = j2 - 1;
					indices.push_back(t);
				}
				else {
					nn = sscanf(consumedline, "%u %u %u%n", &i0, &i1, &i2, &offset);
					if (nn == 3) {
						if (i0 < 0) t.vtxi = vertices.size() + i0; else t.vtxi = i0 - 1;
						if (i1 < 0) t.vtxj = vertices.size() + i1; else t.vtxj = i1 - 1;
						if (i2 < 0) t.vtxk = vertices.size() + i2; else t.vtxk = i2 - 1;
						indices.push_back(t);
					}
					else {
						nn = sscanf(consumedline, "%u//%u %u//%u %u//%u%n", &i0, &k0, &i1, &k1, &i2, &k2, &offset);
						if (i0 < 0) t.vtxi = vertices.size() + i0; else t.vtxi = i0 - 1;
						if (i1 < 0) t.vtxj = vertices.size() + i1; else t.vtxj = i1 - 1;
						if (i2 < 0) t.vtxk = vertices.size() + i2; else t.vtxk = i2 - 1;
						if (k0 < 0) t.ni = normals.size() + k0; else    t.ni = k0 - 1;
						if (k1 < 0) t.nj = normals.size() + k1; else    t.nj = k1 - 1;
						if (k2 < 0) t.nk = normals.size() + k2; else    t.nk = k2 - 1;
						indices.push_back(t);
					}
				}
			}


			consumedline = consumedline + offset;

			while (true) {
				if (consumedline[0] == '\n') break;
				if (consumedline[0] == '\0') break;
				nn = sscanf(consumedline, "%u/%u/%u%n", &i3, &j3, &k3, &offset);
				TriangleIndices t2;
				t2.faceGroup = curGroup;
				if (nn == 3) {
					if (i0 < 0) t2.vtxi = vertices.size() + i0; else    t2.vtxi = i0 - 1;
					if (i2 < 0) t2.vtxj = vertices.size() + i2; else    t2.vtxj = i2 - 1;
					if (i3 < 0) t2.vtxk = vertices.size() + i3; else    t2.vtxk = i3 - 1;
					if (j0 < 0) t2.uvi = uvs.size() + j0; else  t2.uvi = j0 - 1;
					if (j2 < 0) t2.uvj = uvs.size() + j2; else  t2.uvj = j2 - 1;
					if (j3 < 0) t2.uvk = uvs.size() + j3; else  t2.uvk = j3 - 1;
					if (k0 < 0) t2.ni = normals.size() + k0; else   t2.ni = k0 - 1;
					if (k2 < 0) t2.nj = normals.size() + k2; else   t2.nj = k2 - 1;
					if (k3 < 0) t2.nk = normals.size() + k3; else   t2.nk = k3 - 1;
					indices.push_back(t2);
					consumedline = consumedline + offset;
					i2 = i3;
					j2 = j3;
					k2 = k3;
				}
				else {
					nn = sscanf(consumedline, "%u/%u%n", &i3, &j3, &offset);
					if (nn == 2) {
						if (i0 < 0) t2.vtxi = vertices.size() + i0; else    t2.vtxi = i0 - 1;
						if (i2 < 0) t2.vtxj = vertices.size() + i2; else    t2.vtxj = i2 - 1;
						if (i3 < 0) t2.vtxk = vertices.size() + i3; else    t2.vtxk = i3 - 1;
						if (j0 < 0) t2.uvi = uvs.size() + j0; else  t2.uvi = j0 - 1;
						if (j2 < 0) t2.uvj = uvs.size() + j2; else  t2.uvj = j2 - 1;
						if (j3 < 0) t2.uvk = uvs.size() + j3; else  t2.uvk = j3 - 1;
						consumedline = consumedline + offset;
						i2 = i3;
						j2 = j3;
						indices.push_back(t2);
					}
					else {
						nn = sscanf(consumedline, "%u//%u%n", &i3, &k3, &offset);
						if (nn == 2) {
							if (i0 < 0) t2.vtxi = vertices.size() + i0; else    t2.vtxi = i0 - 1;
							if (i2 < 0) t2.vtxj = vertices.size() + i2; else    t2.vtxj = i2 - 1;
							if (i3 < 0) t2.vtxk = vertices.size() + i3; else    t2.vtxk = i3 - 1;
							if (k0 < 0) t2.ni = normals.size() + k0; else   t2.ni = k0 - 1;
							if (k2 < 0) t2.nj = normals.size() + k2; else   t2.nj = k2 - 1;
							if (k3 < 0) t2.nk = normals.size() + k3; else   t2.nk = k3 - 1;
							consumedline = consumedline + offset;
							i2 = i3;
							k2 = k3;
							indices.push_back(t2);
						}
						else {
							nn = sscanf(consumedline, "%u%n", &i3, &offset);
							if (nn == 1) {
								if (i0 < 0) t2.vtxi = vertices.size() + i0; else    t2.vtxi = i0 - 1;
								if (i2 < 0) t2.vtxj = vertices.size() + i2; else    t2.vtxj = i2 - 1;
								if (i3 < 0) t2.vtxk = vertices.size() + i3; else    t2.vtxk = i3 - 1;
								consumedline = consumedline + offset;
								i2 = i3;
								indices.push_back(t2);
							}
							else {
								consumedline = consumedline + 1;
							}
						}
					}
				}
			}

		}


	}
	fclose(f);
}


void Geometry::add_texture(const char* filename) {

	textures.resize(textures.size() + 1);
	w.resize(w.size() + 1);
	h.resize(h.size() + 1);

	FILE* f;
	f = fopen(filename, "rb");
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

	w[w.size() - 1] = *(int*)&info[18]; // extract image height and width from header
	h[h.size() - 1] = *(int*)&info[22];

	int size = 3 * w[w.size() - 1] * h[h.size() - 1];
	textures[textures.size() - 1].resize(size); // allocate 3 bytes per pixel
	fread(&textures[textures.size() - 1][0], sizeof(unsigned char), size, f); // read the rest of the data at once
	fclose(f);

	for (int i = 0; i < size; i += 3) {
		std::swap(textures[textures.size() - 1][i], textures[textures.size() - 1][i + 2]);
	}
}



