#include "header/Bezier_Surface.h"
#include "header/ObjFileParser.h"
#include "header/DeCasteljau.h"
#include "header/Bernstein.h"

int lock = 0;
/* TODO: bei neukalkulierung der bezier kurve auch die kontrollstruktur wieder zentrieren*/
/* TODO: Normalen richtig berechnen */

Bezier_Surface::Bezier_Surface(char* filename, cg::GLSLProgram* prog) : program(prog)
{
	t = 2;
	ObjFileParser parser;
	controlStructure = new PolyObject(prog);
	controlStructure->setColor(PointVector(0.0f, 1.0f, 0.0f, 0.0f));
	parser.parseObjectFile(filename, controlStructure, &deg_m, &deg_n);
	buildControlStructure();

	//v_curves.clear();

	//for (int u = 0; u < u_curves[0]->getCurveVertices().size(); u++) {
	//	vector<PointVector> vcontrol;
	//	PolyObject* v_obj = new PolyObject(program);
	//	v_obj->setColor(PointVector(1.0f, 0.0f, 1.0f, 0.0f));
	//	for (int v = 0; v < u_curves.size(); v++) {
	//		vcontrol.push_back(u_curves[v]->getCurveVertices().at(u));
	//		v_obj->pushVertice(u_curves[v]->getCurveVertices().at(u));
	//		v_obj->pushColor();
	//	}
	//	//v_obj->setVertices(vcontrol);

	//	Bernstein* vcurve = new Bernstein(v_obj, program);
	//	vcurve->setCurveVertices(vcontrol);
	//	v_curves.push_back(vcurve);
	//}

	updateBezierSurface();
	calcNormals();
}

Bezier_Surface::Bezier_Surface(cg::GLSLProgram* prog, vector<CurveBezier*> u_curves_in, vector<CurveBezier*> v_curves_in) : program(prog)
{
	u_curves = u_curves_in;
	v_curves = v_curves_in;
	t = 2;
	ObjFileParser parser;
	controlStructure = new PolyObject(prog);
	vector<int> tmp;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			controlStructure->pushVertice(u_curves[i]->getControlVertices().at(j));
			controlStructure->pushColor();
			tmp.push_back(i * 3 + j);			
		}
		controlStructure->pushIFace(tmp);
		tmp.clear();
		controlStructure->pushFace(u_curves[i]->getControlVertices());
		controlStructure->pushColor(); 
		controlStructure->pushColor();
	}
	controlStructure->pushIndex(0);
	controlStructure->pushIndex(1);
	controlStructure->pushIndex(1);
	controlStructure->pushIndex(2);
	controlStructure->pushIndex(3);
	controlStructure->pushIndex(4);
	controlStructure->pushIndex(4);
	controlStructure->pushIndex(5);
	controlStructure->pushIndex(6);
	controlStructure->pushIndex(7);
	controlStructure->pushIndex(7);
	controlStructure->pushIndex(8);

	deg_m = 2;
	deg_n = 2;
	buildControlStructure();
	updateBezierSurface();
}

Bezier_Surface::~Bezier_Surface()
{
}

PolyObject* Bezier_Surface::getBezierSurface()
{
	return bezierSurface;
}

vector<CurveBezier*> Bezier_Surface::getUCurves()
{
	return u_curves;
}

vector<CurveBezier*> Bezier_Surface::getVCurves()
{
	return v_curves;
}

void Bezier_Surface::init()
{
	for (CurveBezier* curve : u_curves) {
		curve->init();
		//curve->getControlStructure()->init();
		//curve->getControlStructure()->setPoints(true);
		curve->getDerativeStructure()->init();
		//curve->getDeCasteljauStructure()->init();
	}
	for (CurveBezier* curve : v_curves) {
		static int c = 0;
		curve->init();
		//curve->getControlStructure()->init();
		//curve->getControlStructure()->setPoints(true);
		curve->getDerativeStructure()->init();
		c++;
		//curve->getDeCasteljauStructure()->init();
	}

	//normals->init();
	bezierSurface->init();
	controlStructure->init();
	controlStructure->setPoints(true);
}

void Bezier_Surface::draw(bool bezier_toggle, glm::mat4x4 projection, glm::mat4x4 view, glm::mat4x4 model) {
	while (lock == 1) { /* wait */ }
	lock = 1;
	for (CurveBezier* b : u_curves) {
		//Doppelte Verschachtelung nötig, da sonst nicht alle fälle abgedeckt
		if (bezier_toggle) {
			if (dynamic_cast<Bernstein*>(b)) {
				//b->draw(projection * view * model);
				//b->getControlStructure()->draw(projection * view * model);
				//b->getDeCasteljauStructure()->draw(projection * view * model);

				if (b->isDerivative() == 1) {
					b->getDerativeStructure()->draw(projection * view * model);
				}
			}
		}
		else if (dynamic_cast<DeCasteljau*>(b)) {
			//b->draw(projection * view * model);
			//b->getControlStructure()->draw(projection * view * model);
			//b->getDeCasteljauStructure()->draw(projection * view * model);

			if (b->isDerivative() == 1) {
				b->getDerativeStructure()->draw(projection * view * model);
			}
		}
	}

	for (CurveBezier* b : v_curves) {
		//Doppelte Verschachtelung nötig, da sonst nicht alle fälle abgedeckt
		if (bezier_toggle) {
			if (dynamic_cast<Bernstein*>(b)) {
				//b->draw(projection * view * model);
				//b->getControlStructure()->draw(projection * view * model);
				////b->getDeCasteljauStructure()->draw(projection * view * model);

				if (b->isDerivative() == 1) {
					b->getDerativeStructure()->draw(projection * view * model);
					normals->draw(projection * view * model);
				}
			}
		}
		else if (dynamic_cast<DeCasteljau*>(b)) {
			//b->draw(projection * view * model);
			//b->getControlStructure()->draw(projection * view * model);
			////b->getDeCasteljauStructure()->draw(projection * view * model);

			if (b->isDerivative() == 1) {
				b->getDerativeStructure()->draw(projection * view * model);
			}
		}
	}
	controlStructure->draw(projection * view * model);
	bezierSurface->draw(projection * view * model, GL_TRIANGLES);

	lock = 0;
}

void Bezier_Surface::increaseTesselatingRate() {
	if (t < 100) {
		t++;
		updateBezierSurface();
	}
}

void Bezier_Surface::decreaseTesselatingRate() {
	if (t > 1) {
		t--;
		updateBezierSurface();
	}
}

void Bezier_Surface::rationalSurface(int w_i, int w_j, float weight)
{
	vector<GLushort> indices = controlStructure->getIndices();
	vector<PointVector> verts = controlStructure->getVertices();
	vector<vector<PointVector>> faces = controlStructure->getFaces();

	/* Changing the weight of the specified vertice in the controlStructure */
	faces[w_i][w_j].weight = weight;
	faces[w_j * (deg_n + 1) + w_i][w_j].weight = weight;
	verts[(w_i + (deg_m + 1)) + w_j].weight = weight;
	controlStructure->setVertices(verts);
	controlStructure->setFaces(faces);
	controlStructure->setIndices(indices);

	/* Changing the weight of the specified vertice in the u_curve */
	PolyObject* u_obj = u_curves[w_i]->getControlStructure();
	vector<PointVector> u_verts = u_obj->getVertices();
	u_verts[w_j].weight = weight;
	u_obj->setVertices(u_verts);
	u_curves[w_i]->setControlStructure(u_obj);

	/* Changing the weight of the specified vertice in the v_curve */
	PolyObject* v_obj = v_curves[w_j]->getControlStructure();
	vector<PointVector> v_verts = v_obj->getVertices();
	v_verts[w_i].weight = weight;
	v_obj->setVertices(v_verts);
	v_curves[w_j]->setControlStructure(v_obj);

	//buildControlStructure();
	updateBezierSurface();
}


void Bezier_Surface::degree_increase_u()
{
	while (lock == 1) { /* wait */ }
	lock = 1;
	deg_m++;

	/* Cleaning */
	vector<vector<PointVector>> faces = controlStructure->getFaces();
	controlStructure->clearVertices();
	controlStructure->clearFaces();
	controlStructure->clearIndices();
	vector<vector<PointVector>> new_vs(deg_m + 1);

	/* Performing the degree increase on each u-face in the controlStructure */
	for (int i = 0; i <= deg_n; i++) {
		vector<PointVector> face = faces[i];
		PolyObject* obj = new PolyObject(program);
		obj->setVertices(face);
		Bernstein* ucurve = new Bernstein(obj, program);
		ucurve->degree_increase();

		controlStructure->pushFace(ucurve->getControlVertices());
		for (int j = 0; j < ucurve->getControlVertices().size(); j++) {
			controlStructure->pushVertice(ucurve->getControlVertices()[j]);
		}
	}

	/* Rebuilding the controlStructure and bezier surface */
	buildControlStructure();
	lock = 0;
	updateBezierSurface();
}
void Bezier_Surface::degree_increase_v()
{
	while (lock == 1) { /* wait */ }
	lock = 1;
	deg_n++;

	/* Cleaning */
	vector<vector<PointVector>> faces = controlStructure->getFaces();
	controlStructure->clearVertices();
	controlStructure->clearFaces();
	controlStructure->clearIndices();
	vector<PointVector> new_verts((deg_m + 1) * (deg_n + 1));
	/* Performing the degree increase on each u-face in the controlStructure */
	for (int i = 0; i <= deg_m; i++) {
		int index = deg_n + i;
		vector<PointVector> face = faces[deg_n + i];
		PolyObject* obj = new PolyObject(program);
		obj->setVertices(face);
		Bernstein* vcurve = new Bernstein(obj, program);
		vcurve->degree_increase();

		controlStructure->pushFace(vcurve->getControlVertices());
		for (int j = 0; j < vcurve->getControlVertices().size(); j++) {
			int index = (deg_m + 1) * j + i;
			new_verts[(deg_m + 1) * j + i] = vcurve->getControlVertices()[j];
		}
	}
	controlStructure->setVertices(new_verts);

	/* Rebuilding the controlStructure and bezier surface */
	buildControlStructure();
	lock = 0;
	updateBezierSurface();
}

void Bezier_Surface::rotateX()
{
	bezierSurface->rotateX();
	controlStructure->rotateX();
	for (CurveBezier* curve : u_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			//curve->rotateX();
			//curve->getControlStructure()->rotateX();
			//curve->getDeCasteljauStructure()->rotateX();
			curve->getDerativeStructure()->rotateX();
		}
	}
	for (CurveBezier* curve : v_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			//curve->rotateX();
			//curve->getControlStructure()->rotateX();
			//curve->getDeCasteljauStructure()->rotateX();
			curve->getDerativeStructure()->rotateX();
		}
	}
}

void Bezier_Surface::rotateY()
{
	bezierSurface->rotateY();
	controlStructure->rotateY();
	for (CurveBezier* curve : u_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			//curve->rotateY();
			//curve->getControlStructure()->rotateY();
			//curve->getDeCasteljauStructure()->rotateY();
			curve->getDerativeStructure()->rotateY();
		}
	}
	for (CurveBezier* curve : v_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			//curve->rotateY();
			//curve->getControlStructure()->rotateY();
			//curve->getDeCasteljauStructure()->rotateY();
			curve->getDerativeStructure()->rotateY();
		}
	}
}

void Bezier_Surface::rotateZ()
{
	bezierSurface->rotateZ();
	controlStructure->rotateZ();
	for (CurveBezier* curve : u_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			//curve->rotateZ();
			//curve->getControlStructure()->rotateZ();
			//curve->getDeCasteljauStructure()->rotateZ();
			curve->getDerativeStructure()->rotateZ();
		}
	}
	for (CurveBezier* curve : v_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			//curve->rotateZ();
			//curve->getControlStructure()->rotateZ();
			//curve->getDeCasteljauStructure()->rotateZ();
			curve->getDerativeStructure()->rotateZ();
		}
	}
}

void Bezier_Surface::updateCurves() {

	/* Recalculating the bezier curves */
	for (CurveBezier* c : u_curves) {
		c->calcRationalCurve(t);
	}
	for (CurveBezier* c : v_curves) {
		c->calcRationalCurve(t);
	}
}

void Bezier_Surface::calculateVCurves()
{
	/* Cleaning and thread control */
	v_curves.clear();
	while (lock == 1) { /* wait */ }
	lock = 1;

	/* Iterating v_curves */
	for (int v = 0; v <= deg_m; v++) {
		/* Getting the vertices for current v_curve from the controlStructure faces */
		vector<PointVector> vcontrol = controlStructure->getFaces().at(v + deg_n + 1);
		PolyObject* v_obj = new PolyObject(program);
		v_obj->setColor(PointVector(0.0f, 1.0f, 0.0f, 0.0f));
		v_obj->setVertices(vcontrol);
		v_obj->pushColor();

		/* Creating the v_curve */
		Bernstein* vcurve = new Bernstein(v_obj, v_obj->getProgram());
		vcurve->getControlStructure()->setColor(PointVector(0.0f, 1.0f, 0.0f, 0.0f));
		vcurve->calcRationalCurve(t);
		v_curves.push_back(vcurve);
	}

	lock = 0;
}

void Bezier_Surface::calculateUCurves()
{
	/* Cleaning and thread control */
	u_curves.clear();
	while (lock == 1) { /* wait */ }
	lock = 1;
	int n = v_curves[0]->getCurveVertices().size();

	/* Iterating u_curves */
	for (int k = 0; k < n; k++) {

		/* Creating a PolyObject to store the vertices */
		PolyObject* u_obj = new PolyObject(program);
		u_obj->setColor(PointVector(0.0f, 1.0f, 0.0f, 0.0f));

		/* Determine the new u_curve controlVertices based on the curveVertices of the v_curves */
		for (int u = 0; u <= deg_m; u++) {
			u_obj->pushVertice(v_curves[u]->getCurveVertices().at(k));
			u_obj->pushColor();
		}

		/* Creating the u_curve */
		Bernstein* ucurve = new Bernstein(u_obj, u_obj->getProgram());
		ucurve->calcRationalCurve(t);

		u_curves.push_back(ucurve);
	}
	lock = 0;
}

void Bezier_Surface::buildControlStructure() {
	/* Cleaning */
	controlStructure->clearFaces();
	controlStructure->clearIndices();

	/* Calculating the faces based on the given vertices in the controlStructure */
	vector<PointVector> verts = controlStructure->getVertices();
	vector<vector<PointVector>> u_faces(deg_n + 1);
	vector<vector<PointVector>> v_faces(deg_m + 1);
	for (int j = 0; j <= deg_n; j++) {
		for (int i = 0; i <= deg_m; i++) {
			u_faces[j].push_back(verts[j * (deg_m + 1) + i]);
			v_faces[i].push_back(verts[j * (deg_m + 1) + i]);
		}
	}

	/* Pushing the calculated faces into the controlStructure */
	for (vector<PointVector> u_face : u_faces) {
		controlStructure->pushFace(u_face);
	}
	for (vector<PointVector> v_face : v_faces) {
		controlStructure->pushFace(v_face);
	}

	/* Pushing the indices for the v-controlStructures to controlStructure*/
	/* v-indices */
	for (int i = 0; i <= deg_m; i++) {
		for (int j = 0; j < deg_n; j++) {
			controlStructure->pushIndex(j * (deg_m + 1) + i);
			controlStructure->pushIndex((j + 1) * (deg_m + 1) + i);
		}
	}
	/* u-indices */
	for (int i = 0; i <= deg_n; i++) {
		for (int j = 0; j < deg_m; j++) {
			controlStructure->pushIndex(i * (deg_m + 1) + j);
			controlStructure->pushIndex(i * (deg_m + 1) + j + 1);
		}
	}
}

void Bezier_Surface::updateBezierSurface() {
	/* Recalculating the bezier curves */
	calculateVCurves();
	calculateUCurves();

	/* Cleaning */
	//delete bezierSurface;
	bezierSurface = new PolyObject(program);
	bezierSurface->toggleFillSurface();
	bezierSurface->setColor(PointVector(0.0f, 0.0f, 1.0f, 0.0f));

	/* Iterating through the curveVertices of each u_curve */
	for (float i = 0; i < u_curves.size(); i++) {
		int n = u_curves[i]->getCurveVertices().size();

		for (float j = 0; j < n - 1; j++) {

			/* Pushing vertice of u_curves[i][j] into the bezierSurface*/
			bezierSurface->pushVertice(u_curves[i]->getCurveVertices().at(j));
			bezierSurface->pushColor();

			/* Avoid too many indices for the edges */
			/* Leaving the loop if the current vertices is on the right edge */
			if ((n * (i + 1)) + j >= u_curves.size() * n) {
				continue;
			}

			/* Tesselating */

			/* Anti-clock-wise */
			bezierSurface->pushIndex((n * i) + j + 1);
			bezierSurface->pushIndex((n * i) + j);
			bezierSurface->pushIndex((n * (i + 1)) + j);

			bezierSurface->pushIndex((n * i) + j + 1);
			bezierSurface->pushIndex((n * (i + 1)) + j);
			bezierSurface->pushIndex((n * (i + 1)) + j + 1);

			/* Clock-wise */
			bezierSurface->pushIndex((n * i) + j + 1);
			bezierSurface->pushIndex((n * (i + 1)) + j);
			bezierSurface->pushIndex((n * i) + j);

			bezierSurface->pushIndex((n * i) + j + 1);
			bezierSurface->pushIndex((n * (i + 1)) + j + 1);
			bezierSurface->pushIndex((n * (i + 1)) + j);
		}
		bezierSurface->pushVertice(u_curves[i]->getCurveVertices().at(n - 1));
		bezierSurface->pushColor();
	}
}

void Bezier_Surface::subdivisionSurface(float u, float v, vector<CurveBezier*>& u_curves1, vector<CurveBezier*>& v_curves1, vector<CurveBezier*>& u_curves2, vector<CurveBezier*>& v_curves2, vector<CurveBezier*>& u_curves3, vector<CurveBezier*>& v_curves3, vector<CurveBezier*> &u_curves4, vector<CurveBezier*> &v_curves4) {
	PolyObject po = PolyObject(controlStructure->getProgram());

	std::vector<PointVector> tmp1;
	std::vector<PointVector> tmp2;
	std::vector<PointVector> tmp3;
	std::vector<PointVector> tmp4;
	
	subdivision(u, u_curves[0]->getControlVertices(), tmp1, tmp2);	
	u_curves1.push_back(new CurveBezier(tmp1, controlStructure->getProgram()));
	u_curves2.push_back(new CurveBezier(tmp2, controlStructure->getProgram()));

	subdivision(u, u_curves[2]->getControlVertices(), tmp3, tmp4);
	u_curves3.push_back(new CurveBezier(tmp3, controlStructure->getProgram()));
	u_curves4.push_back(new CurveBezier(tmp4, controlStructure->getProgram()));

	subdivision(v, v_curves[0]->getControlVertices(), tmp1, tmp2);
	v_curves1.push_back(new CurveBezier(tmp1, controlStructure->getProgram()));
	v_curves2.push_back(new CurveBezier(tmp2, controlStructure->getProgram()));

	subdivision(v, v_curves[2]->getControlVertices(), tmp3, tmp4);
	v_curves3.push_back(new CurveBezier(tmp3, controlStructure->getProgram()));
	v_curves4.push_back(new CurveBezier(tmp4, controlStructure->getProgram()));

	subdivision(u, u_curves[1]->getControlVertices(), tmp1, tmp2);
	tmp3.clear();
	tmp3.push_back(u_curves1.at(0)->getControlVertices()[2]);
	tmp3.push_back(tmp1.at(2));
	tmp3.push_back(u_curves3.at(0)->getControlVertices()[2]);

	subdivision(v, tmp3, tmp4, tmp2);
	
	v_curves1.push_back(new CurveBezier(tmp4, controlStructure->getProgram()));
	v_curves1.push_back(new CurveBezier(tmp4, controlStructure->getProgram()));
	v_curves2.push_back(new CurveBezier(tmp4, controlStructure->getProgram()));
	v_curves2.push_back(new CurveBezier(tmp4, controlStructure->getProgram()));

	v_curves3.push_back(new CurveBezier(tmp2, controlStructure->getProgram()));
	v_curves3.push_back(new CurveBezier(tmp2, controlStructure->getProgram()));
	v_curves4.push_back(new CurveBezier(tmp2, controlStructure->getProgram()));
	v_curves4.push_back(new CurveBezier(tmp2, controlStructure->getProgram()));

	tmp3.clear();
	tmp3.push_back(v_curves1.at(0)->getControlVertices()[2]);
	tmp3.push_back(tmp4.at(2));
	tmp3.push_back(v_curves3.at(0)->getControlVertices()[2]);

	subdivision(u, tmp3, tmp4, tmp2);

	u_curves1.push_back(new CurveBezier(tmp4, controlStructure->getProgram()));
	u_curves1.push_back(new CurveBezier(tmp4, controlStructure->getProgram()));
	u_curves3.push_back(new CurveBezier(tmp4, controlStructure->getProgram()));
	u_curves3.push_back(new CurveBezier(tmp4, controlStructure->getProgram()));

	u_curves2.push_back(new CurveBezier(tmp2, controlStructure->getProgram()));
	u_curves2.push_back(new CurveBezier(tmp2, controlStructure->getProgram()));
	u_curves4.push_back(new CurveBezier(tmp2, controlStructure->getProgram()));
	u_curves4.push_back(new CurveBezier(tmp2, controlStructure->getProgram()));

	tmp3.clear();
	tmp3.push_back(v_curves1.at(0)->getControlVertices()[1]);
	tmp3.push_back(v_curves1.at(2)->getControlVertices()[1]);
	tmp3.push_back(v_curves3.at(0)->getControlVertices()[1]);

	subdivision(u, tmp3, tmp4, tmp2);
	
	u_curves1[1] = new CurveBezier(tmp4, controlStructure->getProgram());
	u_curves2[1] = new CurveBezier(tmp2, controlStructure->getProgram());
	
	tmp3.clear();
	tmp3.push_back(v_curves4.at(0)->getControlVertices()[1]);
	tmp3.push_back(v_curves4.at(2)->getControlVertices()[1]);
	tmp3.push_back(v_curves2.at(0)->getControlVertices()[1]);

	subdivision(u, tmp3, tmp4, tmp2);

	u_curves3[1] = new CurveBezier(tmp2, controlStructure->getProgram());
	u_curves4[1] = new CurveBezier(tmp4, controlStructure->getProgram());
	
	
	tmp3.clear();
	tmp3.push_back(u_curves1.at(0)->getControlVertices()[1]);
	tmp3.push_back(u_curves1.at(2)->getControlVertices()[1]);
	tmp3.push_back(u_curves3.at(0)->getControlVertices()[1]);

	subdivision(v, tmp3, tmp4, tmp2);

	v_curves1[1] = new CurveBezier(tmp4, controlStructure->getProgram());
	v_curves2[1] = new CurveBezier(tmp2, controlStructure->getProgram());

	tmp3.clear();
	tmp3.push_back(u_curves4.at(0)->getControlVertices()[1]);
	tmp3.push_back(u_curves2.at(0)->getControlVertices()[1]);
	tmp3.push_back(u_curves2.at(0)->getControlVertices()[1]);

	subdivision(v, tmp3, tmp4, tmp2);

	v_curves3[1] = new CurveBezier(tmp2, controlStructure->getProgram());
	v_curves4[1] = new CurveBezier(tmp4, controlStructure->getProgram());


	//Trennung der Flächen zur besseren Ansicht
	PointVector middle = u_curves1.at(2)->getControlVertices()[2];
	PointVector tmpCurve1 = u_curves1.at(0)->getControlVertices()[0] - middle;
	PointVector tmpCurve2 = u_curves2.at(0)->getControlVertices()[0] - middle;
	PointVector tmpCurve3 = u_curves3.at(0)->getControlVertices()[0] - middle;
	PointVector tmpCurve4 = u_curves4.at(0)->getControlVertices()[0] - middle;

	tmpCurve1.normalize();
	tmpCurve2.normalize();
	tmpCurve3.normalize();
	tmpCurve4.normalize();
	
	translatePoint(tmpCurve1, middle, u_curves1);
	translatePoint(tmpCurve2, middle, u_curves2);
	translatePoint(tmpCurve3, middle, u_curves3);
	translatePoint(tmpCurve4, middle, u_curves4);

	translatePoint(tmpCurve1, middle, v_curves1);
	translatePoint(tmpCurve2, middle, v_curves2);
	translatePoint(tmpCurve3, middle, v_curves3);
	translatePoint(tmpCurve4, middle, v_curves4);
}

void Bezier_Surface::translatePoint(PointVector tmpCurve, PointVector middle, vector<CurveBezier*>& curveVerts) {
	vector<PointVector> tmpCons;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			tmpCons.push_back((tmpCurve * 0.2) + curveVerts.at(i)->getControlVertices()[j]);
		}
		curveVerts.at(i) = new CurveBezier(tmpCons, controlStructure->getProgram());
		tmpCons.clear();
	}
}

void Bezier_Surface::subdivision(float t, std::vector<PointVector> & input, std::vector<PointVector> & newVertices1, std::vector<PointVector> & newVertices2) {
	newVertices1.clear();
	newVertices2.clear();
	std::vector<PointVector> vertices = input;

	for (int i = 0; i < vertices.size() - 1; i++) {
		newVertices1.push_back(vertices[0]);
		for (int j = 0; j < vertices.size() - 1 - i; j++) {
			vertices[j] = (vertices[j] * (1 - t)) + (vertices[j + 1] * t);
		}
		newVertices2.push_back(vertices[vertices.size() - 1 - i]);

	}
	newVertices1.push_back(vertices[0]);
	newVertices2.push_back(vertices[0]);
}

//void Bezier_Surface::calculateBezierSurface()
//{
//	bezierSurface = new PolyObject(obj->getProgram());
//	static int tesselate_rate = 3;
//	int k = 0;
//	for (int y = 0; y <= deg_n; y++) {
//		k = y / deg_n;
//		for (int x = 0; x <= (deg_m * tesselate_rate); x++) {
//			vector<PointVector> c_verts_u = u_curves[y]->getCurveVertices();
//			PointVector new_vec = c_verts_u[(c_verts_u.size() / (deg_n * tesselate_rate)) * x];
//			bezierSurface->pushVertice(new_vec);
//
//			//vector<PointVector> c_verts_v = v_curves[k]->getCurveVertices();
//			//new_vec = c_verts_v[(c_verts_v.size() / (deg_m * tesselate_rate)) * y];
//			//bezierSurface->pushVertice(new_vec);
//
//			//if (((tesselate_rate * deg_n) * y + x) % ((tesselate_rate * deg_n) * (y + 1)) != 0 || ((tesselate_rate * deg_n) * y + x) == 0) {
//			if (y < (deg_n * tesselate_rate) && x < (deg_m * tesselate_rate)) {
//				bezierSurface->pushIndex((tesselate_rate * deg_n + 1) * y + x);
//				bezierSurface->pushIndex((tesselate_rate * deg_n + 1) * y + x + 1);
//				bezierSurface->pushIndex((tesselate_rate * deg_n + 1) * (y + 1) + x);
//
//				bezierSurface->pushIndex((tesselate_rate * deg_n + 1) * y + x + 1);
//				bezierSurface->pushIndex((tesselate_rate * deg_n + 1) * (y + 1) + x + 1);
//				bezierSurface->pushIndex((tesselate_rate * deg_n + 1) * (y + 1) + x);
//
//				//}
//			}
//			bezierSurface->pushColor();
//			//bezierSurface_vertices.push_back(u_curves[x + 1]->getCurveVertices().at((c_verts_u.size() / deg_n) * (y + 1)));
//		}
//	}
//
//	//}
//	//tesselate_rate++;
//	//bezierSurface->pushIndex(0);
//	//bezierSurface->pushIndex(1);
//	//bezierSurface->pushIndex(3);
//	//bezierSurface->pushIndex(1);
//	//bezierSurface->pushIndex(4);
//	//bezierSurface->pushIndex(3);
//	//bezierSurface->pushIndex(1);
//	//bezierSurface->pushIndex(2);
//	//bezierSurface->pushIndex(4);
//	//bezierSurface->pushIndex(2);
//	//bezierSurface->pushIndex(5);
//	//bezierSurface->pushIndex(4);
//
//	//bezierSurface->pushIndex(3);
//	//bezierSurface->pushIndex(4);
//	//bezierSurface->pushIndex(6);
//	//bezierSurface->pushIndex(4);
//	//bezierSurface->pushIndex(7);
//	//bezierSurface->pushIndex(6);
//	//bezierSurface->pushIndex(4);
//	//bezierSurface->pushIndex(5);
//	//bezierSurface->pushIndex(7);
//	//bezierSurface->pushIndex(5);
//	//bezierSurface->pushIndex(8);
//	//bezierSurface->pushIndex(7);
//
//
//}

void Bezier_Surface::calcNormals() {
	normals = new PolyObject(program);
	normals->setColor(PointVector(1.0f, 1.0f, 0.0f, 0.0f));

	for (int i = 0; i < u_curves.size(); i++) {
		PolyObject* d_u = u_curves[i]->getDerativeStructure();

		for (int j = 0; j < v_curves.size(); j++) {
			PolyObject* d_v = v_curves[j]->getDerativeStructure();

			for (int k = 0; k < v_curves[j]->getDerativeStructure()->getVertices().size(); k++) {
				PointVector normal;

				vector<PointVector> verts_u = d_u->getVertices();
				vector<PointVector> verts_v = d_v->getVertices();

				if (k % 2 == 0 && k != 0) {
					PointVector du = verts_u[k] - verts_u[k - 1];
					PointVector dv = verts_v[k] - verts_v[k - 1];
					normal.xCoor = du.xCoor * dv.xCoor;
					normal.yCoor = du.yCoor * dv.yCoor;
					normal.zCoor = du.zCoor * dv.zCoor;

					normals->pushVertice(du + dv);
					normals->pushVertice(normal + normal);
					normals->pushIndex(k - 1);
					normals->pushIndex(k);
					normals->pushColor();
				}
			}
		}
	}
}
