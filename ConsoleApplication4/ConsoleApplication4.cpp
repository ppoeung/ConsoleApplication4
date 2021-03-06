#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

//

#include <iostream>
#include <Vector>
#include <math.h>
#include <algorithm>    // std::max
#include <vector>
#include <random>
#include <time.h>

#include "Ray.h"
#include "Vector.h"
#include "Sphere.h"
#include "Scene.h"
#include "Triangle.h"
#include "Geometry.h"

static std::default_random_engine engine;
static std::uniform_real_distribution<double> u(0, 1);


using namespace std;

void save_image(const char* filename, const unsigned char* tableau, int w, int h) { // (0,0) is top-left corner

	FILE *f;

	int filesize = 54 + 3 * w*h;

	unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
	unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };
	unsigned char bmppad[3] = { 0,0,0 };

	bmpfileheader[2] = (unsigned char)(filesize);
	bmpfileheader[3] = (unsigned char)(filesize >> 8);
	bmpfileheader[4] = (unsigned char)(filesize >> 16);
	bmpfileheader[5] = (unsigned char)(filesize >> 24);

	bmpinfoheader[4] = (unsigned char)(w);
	bmpinfoheader[5] = (unsigned char)(w >> 8);
	bmpinfoheader[6] = (unsigned char)(w >> 16);
	bmpinfoheader[7] = (unsigned char)(w >> 24);
	bmpinfoheader[8] = (unsigned char)(h);
	bmpinfoheader[9] = (unsigned char)(h >> 8);
	bmpinfoheader[10] = (unsigned char)(h >> 16);
	bmpinfoheader[11] = (unsigned char)(h >> 24);

	f = fopen(filename, "wb");
	fwrite(bmpfileheader, 1, 14, f);
	fwrite(bmpinfoheader, 1, 40, f);
	unsigned char *row = new unsigned char[w * 3];
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++) {
			row[j * 3] = tableau[(w*(h - i - 1) * 3) + j * 3 + 2];
			row[j * 3 + 1] = tableau[(w*(h - i - 1) * 3) + j * 3 + 1];
			row[j * 3 + 2] = tableau[(w*(h - i - 1) * 3) + j * 3];
		}
		fwrite(row, 3, w, f);
		fwrite(bmppad, 1, (4 - (w * 3) % 4) % 4, f);
	}
	fclose(f);
	delete[] row;
}

//bool intersect(const Ray &ray, const Sphere &s) {






int main()
{
	clock_t tStart = clock();

	int W = 1024;
	std::vector<unsigned char> img(W*W * 3, 0);

	double pi = 3.1415926;

	Vector eye = Vector(0, 0, 55);
	double fov = 60 * pi / 180;
	Vector sourceLum = Vector(-10, 20, 40);
	double I = 5000000000;
	double focus_distance = 55;
	double ouverture = 0.2;


	Scene scene(sourceLum, I);


	Sphere s1 = Sphere(Vector(-25, 0, -6), 12, Vector(1, 0, 0.2), false, false); // sphere1
	Sphere s2 = Sphere(Vector(-15, 30, -30), 6, Vector(1, 0.1, 0.6), false, false); // sphere 2
	Sphere s3 = Sphere(Vector(18, 15, -10), 10, Vector(0.4, 0.1, 0.6), false, false); // sphere 2
	Sphere sDroite = Sphere(Vector(-100000, 0, 0), 99940, Vector(0.8, 0.5, 0.6));//mur droite
	Sphere sGauche = Sphere(Vector(100000, 0, 0), 99940, Vector(0.8, 0.5, 0.6));//mur gauche
	Sphere sSol = Sphere(Vector(0, -100000, 0), 99960, Vector(0.8, 0.5, 0.6));//sol
	Sphere sPlafond = Sphere(Vector(0, 100200, 0), 99960, Vector(0.8, 0.5, 0.6));//plafond
	Sphere sFond = Sphere(Vector(0, 0, -100000), 99900, Vector(0.8, 0.5, 0.6)); //fond
	Sphere sDerriere = Sphere(Vector(0, 0, 100000), 99940, Vector(0.8, 0.5, 0.6)); //derriere camera

	Sphere sLum = Sphere(Vector(0, 80, 30), 4, Vector(0, 0, 0), false, false, true);

	scene.addObject(sLum);

	scene.addObject(s1);
	scene.addObject(s2);
	scene.addObject(s3);
	scene.addObject(sDroite);
	scene.addObject(sGauche);
	scene.addObject(sSol);
	scene.addObject(sPlafond);
	scene.addObject(sFond);
	scene.addObject(sDerriere);

	Geometry girl = Geometry("girl.obj", 18., Vector(0, -21, 0), Vector(0.6, 0.6, 0.6));

	scene.addObject(girl);
	girl.add_texture("text head.bmp");
	girl.add_texture("text hair.bmp");
	girl.add_texture("text torso.bmp");
	girl.add_texture("text legs.bmp");
	girl.add_texture("text foot.bmp");
	girl.add_texture("text hand.bmp");

	int NRay = 20;
	int NRebond = 5;

	Vector P;
	Vector N;
	int numSph;
	double t;
	Vector colorIntersect;

	double z = -W / 2 / tan(fov / 2);

#pragma omp parallel for
	for (int j = 0; j < W; j += 1) {
		std::cout << 100 * j / W << "%... " << (double)(clock() - tStart) / CLOCKS_PER_SEC << endl;
		for (int i = 0; i < W; i += 1) {
			Vector Ipixel = Vector(0, 0, 0);
			for (int nRay = 1; nRay <= NRay; nRay++) {
				double u1 = u(engine);
				double u2 = u(engine);
				double rx = sqrt(-2 * log(u1))*cos(2 * pi*u2)*0.25;
				double ry = sqrt(-2 * log(u1))*sin(2 * pi*u2)*0.25;
				double u3 = u(engine);
				double u4 = u(engine);
				double rxo = sqrt(-2 * log(u3))*cos(2 * pi*u4)*ouverture;
				double ryo = sqrt(-2 * log(u3))*sin(2 * pi*u4)*ouverture;
				Vector direction = getNormalized(Vector(i - W / 2 + rx, j - W / 2 + ry, z)); // c'est à la direction (corrigée par l'A-A)
				Vector destination = eye + focus_distance*direction;
				Vector eye_decale = eye + Vector(rxo, ryo, 0); // c'est l'oeil de la caméra modifée pour le depth of field
				Ray ray = Ray(eye_decale, getNormalized(destination - eye_decale));
				if (scene.intersect(ray, P, N, numSph, t, colorIntersect)) {       //test de si le ray touche au moins un objet	
					Ipixel += scene.getColor(ray, NRebond) / NRay;
				}
			}
			img[((W - j - 1)*W + i) * 3 + 0] = std::max(0., std::min(255., pow(Ipixel[0], 0.45)));
			img[((W - j - 1)*W + i) * 3 + 1] = std::max(0., std::min(255., pow(Ipixel[1], 0.45)));
			img[((W - j - 1)*W + i) * 3 + 2] = std::max(0., std::min(255., pow(Ipixel[2], 0.45)));
		}
	}
	
	save_image("test.bmp", &img[0], W, W);



	printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
	return 0;

}

