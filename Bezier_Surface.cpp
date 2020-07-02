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


Bezier_Surface::Bezier_Surface(PolyObject *controlStructure, int deg_n, int deg_m, int t, cg::GLSLProgram *prog) : program(prog) {
	this->deg_n = deg_n;
	this->deg_m = deg_m;
	this->controlStructure = controlStructure;
	this->t = t;
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
		buildControlStructure();
		updateBezierSurface();
	}
}

void Bezier_Surface::decreaseTesselatingRate() {
	if (t > 1) {
		t--;
		buildControlStructure();
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
			curve->rotateX();
			curve->getControlStructure()->rotateX();
			//curve->getDeCasteljauStructure()->rotateX();
			curve->getDerativeStructure()->rotateX();
		}
	}
	for (CurveBezier* curve : v_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			curve->rotateX();
			curve->getControlStructure()->rotateX();
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
			curve->rotateY();
			curve->getControlStructure()->rotateY();
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
			curve->rotateZ();
			curve->getControlStructure()->rotateZ();
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

void Bezier_Surface::subdivideU(float u, float v, vector<Bezier_Surface*>* allSurfaces) {

	PolyObject* newPo = new PolyObject(program);
	Bezier_Surface* newSurface = new Bezier_Surface(newPo, deg_m, deg_n, this->t, program);

	vector<CurveBezier*> newUCurves;

	for (CurveBezier* c : u_curves) {
		vector<PointVector> newCurve;
		newCurve.clear();

		c->subdivision(u, newCurve);

		PolyObject* controlPoints = new PolyObject(program);
		controlPoints->setVertices(newCurve);
		for (int i = 0; i < newCurve.size() - 1; i++) {
			controlPoints->pushIndex(i);
			controlPoints->pushIndex(i + 1);
			controlPoints->pushColor();
		}
		PolyObject* po = new PolyObject(program);
		po->setVertices(newCurve);
		Bernstein* b = new Bernstein(controlPoints, program);
		b->obj->setVertices(newCurve);
		b->setControlStructure(po);
		b->setInitialized(false);
		newUCurves.push_back(b);
	}

	for (CurveBezier* c : newUCurves) {
		newSurface->u_curves.push_back(c);
	}
	for (CurveBezier* c : v_curves) {
		PolyObject* controlPoints = new PolyObject(program);
		controlPoints->setVertices(c->getControlVertices());
		for (int i = 0; i < c->getControlVertices().size() - 1; i++) {
			controlPoints->pushIndex(i);
			controlPoints->pushIndex(i + 1);
			controlPoints->pushColor();
		}
		PolyObject* po = new PolyObject(program);
		po->setVertices(c->getControlVertices());
		Bernstein* b = new Bernstein(controlPoints, program);
		b->obj->setVertices(c->getControlVertices());
		b->setControlStructure(po);
		b->setInitialized(false);
		b->calcRationalCurve(t);
		newSurface->v_curves.push_back(b);
	}

	controlStructure->clearVertices();
	for (int i = 0; i < deg_m + 1; i++) {
		for (int j = 0; j < deg_n + 1; j++) {
			newSurface->controlStructure->pushVertice(newUCurves[i]->getControlVertices()[j]);
			newSurface->controlStructure->pushColor();
			controlStructure->pushVertice(u_curves[i]->getControlVertices()[j]);
		}
	}
	buildControlStructure();
	updateBezierSurface();
	newSurface->buildControlStructure();
	newSurface->updateBezierSurface();
	allSurfaces->push_back(newSurface);

	subdivideV(v, allSurfaces);
	newSurface->subdivideV(v, allSurfaces);
}

void Bezier_Surface::subdivideV(float v, vector<Bezier_Surface*>* allSurfaces) {

	PolyObject* newPo = new PolyObject(program);
	Bezier_Surface* newSurface = new Bezier_Surface(newPo, deg_m, deg_n, this->t, program);

	vector<CurveBezier*> newVCurves;

	for (CurveBezier* c : v_curves) {
		vector<PointVector> newCurve;
		newCurve.clear();

		c->subdivision(v, newCurve);

		PolyObject* controlPoints = new PolyObject(program);
		controlPoints->setVertices(newCurve);
		for (int i = 0; i < newCurve.size() - 1; i++) {
			controlPoints->pushIndex(i);
			controlPoints->pushIndex(i + 1);
			controlPoints->pushColor();
		}
		PolyObject* po = new PolyObject(program);
		po->setVertices(newCurve);
		Bernstein* b = new Bernstein(controlPoints, program);
		b->obj->setVertices(newCurve);
		b->setControlStructure(po);
		b->setInitialized(false);
		newVCurves.push_back(b);
	}

	for (CurveBezier* c : newVCurves) {
		newSurface->v_curves.push_back(c);
	}
	for (CurveBezier* c : u_curves) {
		PolyObject* controlPoints = new PolyObject(program);
		controlPoints->setVertices(c->getControlVertices());
		for (int i = 0; i < c->getControlVertices().size() - 1; i++) {
			controlPoints->pushIndex(i);
			controlPoints->pushIndex(i + 1);
			controlPoints->pushColor();
		}
		PolyObject* po = new PolyObject(program);
		po->setVertices(c->getControlVertices());
		Bernstein* b = new Bernstein(controlPoints, program);
		b->obj->setVertices(c->getControlVertices());
		b->setControlStructure(po);
		b->setInitialized(false);
		b->calcRationalCurve(t);
		newSurface->u_curves.push_back(b);
	}

	controlStructure->clearVertices();
	for (int i = 0; i < deg_m + 1; i++) {
		for (int j = 0; j < deg_n + 1; j++) {
			newSurface->controlStructure->pushVertice(newVCurves[i]->getControlVertices()[j]);
			newSurface->controlStructure->pushColor();
			controlStructure->pushVertice(v_curves[i]->getControlVertices()[j]);
		}
	}
	buildControlStructure();
	updateBezierSurface();
	newSurface->buildControlStructure();
	newSurface->updateBezierSurface();

	allSurfaces->push_back(newSurface);
}