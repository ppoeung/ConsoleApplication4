#include "Scene.h"

Scene::Scene(Vector sourceLum, double I) {
	this->sourceLum = sourceLum;
	this->I = I;
}


bool Scene::intersect(const Ray &ray, Vector &P, Vector &N, int &numSphere, double &tt, Vector &color) {
	bool has_inter = false;
	double tmin = 1E99;
	double t;
	Vector localP;
	Vector localN;
	Vector localColor;

	for (int i = 0; i < objects.size(); i++) {

		bool local_has_inter = objects[i]->intersect(ray, localP, localN, t, localColor);
		if (local_has_inter)
		{
			has_inter = true;
			if (t < tmin) {
				tmin = t;
				P = localP;
				N = localN;
				numSphere = i;
				color = localColor;
			}
		}
	}
	tt = tmin;
	return has_inter;
};

Vector Scene::getColor(Ray ray, int nbRec) {
	Vector P = Vector(0, 0, 0);
	Vector N;
	int numSph;
	double t;
	Vector color;
	Vector Ipixel;
	bool inter = intersect(ray, P, N, numSph, t, color);


	Vector PCS;
	Vector NCS;
	int numSphCS;
	double tCS;
	double epsilonCS = 0.01;
	bool CS;

	if (nbRec < 0 || !inter) { //fin de récursion OU pas d'intersection
		return Vector(0, 0, 0);
	}

	Vector Ipix = Vector(0, 0, 0);

	if (!objects[numSph]->isMirror && !objects[numSph]->isTransparent) { //cas général
																		 //Ipix += I*color*std::max(0., prodSca(getNormalized(sourceLum - P), N)) / (sourceLum - P).getNorm2();

																		 /*if (numSph == 7) {
																		 //	std:cout << color[0] << " " << color[1] << " " << color[2] << endl;
																		 }*/

		Vector O_lum = dynamic_cast<const Sphere*>(objects[0])->O;
		double r_lum = dynamic_cast<const Sphere*>(objects[0])->r;
		Vector OP = P - O_lum;
		Vector dirOP = getNormalized(OP);
		Vector Pprime = O_lum + r_lum*dirAlea(dirOP); //point à la surface de la demi sphere lumineuse généré aléatoirement
		Vector Nprime = getNormalized(Pprime - O_lum); //
		Vector colorCS;
		Vector PPprime = Pprime - P;
		Vector dirPPprime = getNormalized(PPprime);
		double PPprimeNorm2 = PPprime.getNorm2();
		double proba = prodSca(dirOP, Nprime); //probabilité de générer ce point * pi
		bool CS = intersect(Ray(P + epsilonCS*N, PPprime), PCS, NCS, numSphCS, tCS, colorCS);


		if (!CS || (CS && tCS > 0.99*std::sqrt(PPprimeNorm2))) {              //éclairage direct
			Ipix += I*color / 3.1415926 / proba*(std::max(0., -prodSca(dirPPprime, Nprime)))*(std::max(0., prodSca(dirPPprime, N))) / PPprimeNorm2;

		}

		Vector PP;
		Vector NN;
		int numnumSph = 100000;
		double tt;
		Vector dir_Alea = dirAlea(N);
		Vector color_osef;
		bool osef = intersect(Ray(P, dir_Alea), PP, NN, numnumSph, tt, color_osef);
		if (osef & numnumSph != 0) { //test pour savoir si le rayon aleatoire pointe vers la source lumineuse
			Ipix += prodHad(color, getColor(Ray(P + 0.00001*N, dir_Alea), nbRec - 1)); // éclairage aléatioire 
		}

	}

	else if (objects[numSph]->isMirror) { //cas mirroir  
		Ipix += getColor(Ray(P + 0.001*N, reflected(ray.u, N)), nbRec - 1);
	}

	else if (objects[numSph]->isTransparent) { //cas transparent  
		double n_air = 1;
		double n_glass = 1.2;
		double n1;
		double n2;
		Vector norm_trans;
		if (prodSca(ray.u, N) < 0) {
			n1 = n_air;
			n2 = n_glass;
			norm_trans = N;
		}
		else if (prodSca(ray.u, N) > 0) {
			n1 = n_glass;
			n2 = n_air;
			norm_trans = -1 * N;
		}
		double radicand = 1 - n1*n1 / n2 / n2*(1 - pow(prodSca(ray.u, norm_trans), 2));;
		if (radicand > 0) {
			Vector refracted = n1 / n2*(ray.u - prodSca(ray.u, norm_trans)*norm_trans) - sqrt(radicand)*norm_trans;
			Ipix += getColor(Ray(P - 0.00001*norm_trans, refracted), nbRec -1); //remetre nbRec-1
		}
	}

	return Ipix;
}

Scene::~Scene()
{
}
