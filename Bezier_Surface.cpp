#include "header/Bezier_Surface.h"
#include "header/ObjFileParser.h"
#include "header/DeCasteljau.h"
#include "header/Bernstein.h"
#include "header/Bezier_Operations.h"

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

	updateBezierSurface();
	calcNormals();
}


Bezier_Surface::Bezier_Surface(PolyObject* controlStructure, int deg_m, int deg_n, int t, cg::GLSLProgram* prog) : program(prog) {
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
		curve->getDerativeStructure()->init();
	}
	for (CurveBezier* curve : v_curves) {
		static int c = 0;
		curve->init();
		curve->getDerativeStructure()->init();
		c++;
	}

	normal->init();
	u_deriv->init();
	v_deriv->init();
	normals->init();
	bezierSurface->init();
	controlStructure->init();
	controlStructure->setPoints(true);
}

void Bezier_Surface::draw(bool bezier_toggle, glm::mat4x4 projection, glm::mat4x4 view, glm::mat4x4 model) {
	while (lock == 1) { /* wait */ }
	lock = 1;

	if (showNormals) {
		normals->draw(projection * view * model);
	}
	if (showUVnormal) {
		normal->draw(projection * view * model);
		u_deriv->draw(projection * view * model);
		v_deriv->draw(projection * view * model);
	}
	if (drawSurfaceControlStructure) controlStructure->draw(projection * view * model);
	bezierSurface->draw(projection, view, model, GL_TRIANGLES);

	lock = 0;
}

void Bezier_Surface::increaseTesselatingRate() {
	if (t < 100) {
		t++;
		buildControlStructure();
		updateBezierSurface();
		calcNormals();
	}
}

void Bezier_Surface::decreaseTesselatingRate() {
	if (t > 1) {
		t--;
		buildControlStructure();
		updateBezierSurface();
		calcNormals();
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
		CurveBezier* ucurve = new CurveBezier(obj, program);
		if (bezier_toggle) {
			dynamic_cast<Bernstein*>(ucurve);
			ucurve = new Bernstein(obj, program);
		}
		else {
			dynamic_cast<DeCasteljau*>(ucurve);
			ucurve = new DeCasteljau(obj, program);
		}
		ucurve->degree_increase();

		controlStructure->pushFace(ucurve->getControlVertices());
		for (int j = 0; j < ucurve->getControlVertices().size(); j++) {
			controlStructure->pushVertice(ucurve->getControlVertices()[j]);
			controlStructure->pushColor();
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
		CurveBezier* vcurve = new CurveBezier(obj, program);
		if (bezier_toggle) {
			dynamic_cast<Bernstein*>(obj);
			vcurve = new Bernstein(obj, program);
		}
		else {
			dynamic_cast<DeCasteljau*>(obj);
			vcurve = new DeCasteljau(obj, program);
		}
		vcurve->degree_increase();

		controlStructure->pushFace(vcurve->getControlVertices());
		for (int j = 0; j < vcurve->getControlVertices().size(); j++) {
			int index = (deg_m + 1) * j + i;
			new_verts[(deg_m + 1) * j + i] = vcurve->getControlVertices()[j];
			controlStructure->pushColor();
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
	normal->rotateX();
	u_deriv->rotateX();
	v_deriv->rotateX();
	bezierSurface->rotateX();
	controlStructure->rotateX();
	normals->rotateX();

	for (CurveBezier* curve : u_curves) {
			curve->rotateX();
			curve->getControlStructure()->rotateX();
			//curve->getDeCasteljauStructure()->rotateX();
			curve->getDerativeStructure()->rotateX();
	}
	for (CurveBezier* curve : v_curves) {
			curve->rotateX();
			curve->getControlStructure()->rotateX();
			//curve->getDeCasteljauStructure()->rotateX();
			curve->getDerativeStructure()->rotateX();
	}
}

void Bezier_Surface::rotateY()
{
	normal->rotateY();
	u_deriv->rotateY();
	v_deriv->rotateY();
	normals->rotateY();
	bezierSurface->rotateY();
	controlStructure->rotateY();

	for (CurveBezier* curve : u_curves) {
		curve->rotateY();
		curve->getControlStructure()->rotateY();
		//curve->getDeCasteljauStructure()->rotateY();
		curve->getDerativeStructure()->rotateY();
	}
	for (CurveBezier* curve : v_curves) {
		curve->rotateY();
		curve->getControlStructure()->rotateY();
		//curve->getDeCasteljauStructure()->rotateY();
		curve->getDerativeStructure()->rotateY();
	}
}

void Bezier_Surface::rotateZ()
{
	normal->rotateZ();
	u_deriv->rotateZ();
	v_deriv->rotateZ();
	normals->rotateZ();
	bezierSurface->rotateZ();
	controlStructure->rotateZ();

	for (CurveBezier* curve : u_curves) {
		curve->rotateZ();
		curve->getControlStructure()->rotateZ();
		//curve->getDeCasteljauStructure()->rotateZ();
		curve->getDerativeStructure()->rotateZ();
	}
	for (CurveBezier* curve : v_curves) {
		curve->rotateZ();
		curve->getControlStructure()->rotateZ();
		//curve->getDeCasteljauStructure()->rotateZ();
		curve->getDerativeStructure()->rotateZ();
	}
}

void Bezier_Surface::translate(PointVector v) {
	normals->translate(v);
	bezierSurface->translate(v);
	controlStructure->translate(v);

	for (CurveBezier* curve : u_curves) {
		curve->translate(v);
		curve->getControlStructure()->translate(v);
		curve->getDerativeStructure()->translate(v);
	}
	for (CurveBezier* curve : v_curves) {
		curve->translate(v);
		curve->getControlStructure()->translate(v);
		curve->getDerativeStructure()->translate(v);
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
		CurveBezier* vcurve = new CurveBezier(v_obj, v_obj->getProgram());
		if (bezier_toggle) {
			dynamic_cast<Bernstein*>(vcurve);
			vcurve = new Bernstein(v_obj, program);
		}
		else {
			dynamic_cast<DeCasteljau*>(vcurve);
			vcurve = new DeCasteljau(v_obj, program);
		}
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
		CurveBezier* ucurve = new CurveBezier(u_obj, program);
		if (bezier_toggle) {
			dynamic_cast<Bernstein*>(ucurve);
			ucurve = new Bernstein(u_obj, u_obj->getProgram());
		}
		else {
			dynamic_cast<DeCasteljau*>(ucurve);
			ucurve = new DeCasteljau(u_obj, u_obj->getProgram());
		}
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
	calcTangent();

	/* Cleaning */
	if (bezierSurface != nullptr) {
		bezierSurface->clearIndices();
		bezierSurface->clearStructure();
		bezierSurface->clearVertices();
		bezierSurface->clearNormals();
		bezierSurface->clearColors();
	}
	else {
		bezierSurface = new PolyObject(program);
	}
	if (dynamic_cast<Bernstein*>(u_curves[0])) {
		bezierSurface->setColor(PointVector(1.0f, 0.75f, 0.0f, 0.0f));
	}
	else if (dynamic_cast<DeCasteljau*>(u_curves[0])) {
		bezierSurface->setColor(PointVector(1.0f, 1.0f, 0.0f, 0.0f));
	}

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

			/* 1st Triangle Anti-clock-wise */
			//bezierSurface->pushIndex((n * i) + j + 1);
			//bezierSurface->pushIndex((n * i) + j);
			//bezierSurface->pushIndex((n * (i + 1)) + j);

			///* 1st Triangle Clock-wise */
			bezierSurface->pushIndex((n * i) + j + 1);
			bezierSurface->pushIndex((n * (i + 1)) + j);
			bezierSurface->pushIndex((n * i) + j);


			/* 2nd Triangle Anti-clock-wise */
			//bezierSurface->pushIndex((n * i) + j + 1);
			//bezierSurface->pushIndex((n * (i + 1)) + j);
			//bezierSurface->pushIndex((n * (i + 1)) + j + 1);

			///* 2nd Triangle Clock-wise */
			bezierSurface->pushIndex((n * i) + j + 1);
			bezierSurface->pushIndex((n * (i + 1)) + j + 1);
			bezierSurface->pushIndex((n * (i + 1)) + j);
		}
		bezierSurface->pushVertice(u_curves[i]->getCurveVertices().at(n - 1));
		bezierSurface->pushColor();
	}
}

void Bezier_Surface::subdivision(float t, std::vector<PointVector>& input, std::vector<PointVector>& newVertices1, std::vector<PointVector>& newVertices2) {
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

PointVector Bezier_Surface::getNormal(float u, float v) {
	vector<PointVector> u_verts;
	vector<PointVector> v_verts;

	for (int i = 0; i < v_curves.size(); i++) {
		vector<PointVector> u_curve;
		for (int j = 0; j < v_curves[0]->getControlVertices().size(); j++) {
			u_curve.push_back(v_curves[j]->getControlVertices()[i]);
		}
		v_verts.push_back(calcPoint_bernstein(u_curve, u));
		u_verts.push_back(calcPoint_bernstein(v_curves[i]->getControlVertices(), v));
	}

	PointVector ca = calcDerivative_bernstein(u_verts, u) - calcPoint_bernstein(u_verts, u);
	PointVector cb = calcDerivative_bernstein(v_verts, v) - calcPoint_bernstein(v_verts, v);
	PointVector norm = ca.crossProduct(cb);
	norm.normalize();
	return norm;
}

PointVector Bezier_Surface::getPoint(float u, float v) {
	vector<PointVector> u_verts;

	for (int i = 0; i < v_curves.size(); i++) {
		vector<PointVector> u_curve;
		for (int j = 0; j < v_curves[0]->getControlVertices().size(); j++) {
			u_curve.push_back(v_curves[j]->getControlVertices()[i]);
		}
		u_verts.push_back(calcPoint_bernstein(v_curves[i]->getControlVertices(), v));
	}

	return calcPoint_bernstein(u_verts, u);
}

void Bezier_Surface::calcTangent() {
	if (u_deriv != nullptr)
		delete u_deriv;
	if (v_deriv != nullptr)
		delete v_deriv;
	if (normal != nullptr)
		delete normal;

	u_deriv = new PolyObject(program);
	v_deriv = new PolyObject(program);
	normal = new PolyObject(program);

	vector<PointVector> u_verts;
	vector<PointVector> v_verts;

	for (int i = 0; i < v_curves.size(); i++) {
		u_verts.push_back(calcPoint_bernstein(v_curves[i]->getControlVertices(), v_der));
	}

	for (int i = 0; i < v_curves[0]->getControlVertices().size(); i++) {
		vector<PointVector> u_curve;
		for (int j = 0; j < v_curves.size(); j++) {
			u_curve.push_back(v_curves[j]->getControlVertices()[i]);
		}
		v_verts.push_back(calcPoint_bernstein(u_curve, u_der));
	}

	/* Derivative in u-direction */
	u_deriv->pushVertice(calcPoint_bernstein(v_verts, v_der));
	u_deriv->pushVertice(calcDerivative_bernstein(v_verts, v_der));
	u_deriv->pushColor(PointVector(1.0f, 0.0f, 0.0f, 0.0f));
	u_deriv->pushColor(PointVector(1.0f, 0.0f, 0.0f, 0.0f));

	/* Derivative in v-direction */
	v_deriv->pushVertice(calcPoint_bernstein(u_verts, u_der));
	v_deriv->pushVertice(calcDerivative_bernstein(u_verts, u_der));
	v_deriv->pushColor(PointVector(0.0f, 0.0f, 1.0f, 0.0f));
	v_deriv->pushColor(PointVector(0.0f, 0.0f, 1.0f, 0.0f));

	/* Fill the derivatives with indices */
	for (int i = 0; i < 2; i++) {
		u_deriv->pushIndex(i);
		v_deriv->pushIndex(i);
		normal->pushIndex(i);
	}
	PointVector ca = v_deriv->getVertices()[1] - v_deriv->getVertices()[0];
	PointVector cb = u_deriv->getVertices()[1] - u_deriv->getVertices()[0];

	normal->pushVertice(v_deriv->getVertices()[0]);
	PointVector n = ca.crossProduct(cb);
	n.normalize();
	n = n * 3;
	normal->pushVertice(v_deriv->getVertices()[0] + n);
	normal->pushColor(PointVector(1.0f, 0.0f, 1.0f, 0.0f));
	normal->pushColor(PointVector(1.0f, 0.0f, 1.0f, 0.0f));
}

void Bezier_Surface::calcNormals() {
	bezierSurface->clearNormals();
	if (normals != nullptr)
		delete normals;
	normals = new PolyObject(program);
	normals->setColor(PointVector(1.0f, 1.0f, 0.0f, 0.0f));
	vector<PointVector> vertices = bezierSurface->getVertices();
	vector<GLushort> indices = bezierSurface->getIndices();
	int k = 0;

	for (int i = 0; i < vertices.size(); i++) {
		int anzahl = 0;

		for (int j = 0; j < indices.size(); j++) 
			if (&vertices[i] == &vertices[indices[j]]) anzahl++;

		PointVector normal;
		float faktor = 1.0f / (float)anzahl;

		for (int j = 0; j < indices.size(); j++) {
			if (&vertices[i] == &vertices[indices[j]]) {
				PointVector n;
				PointVector a;
				PointVector b;

				if (j % 3 == 0) {
					a = vertices[indices[j + 2]] - vertices[i];
					b = vertices[indices[j + 1]] - vertices[i];
				}
				else if (j % 3 == 1) {
					a = vertices[indices[j - 1]] - vertices[i];
					b = vertices[indices[j + 1]] - vertices[i];
				}
				else {
					a = vertices[indices[j - 1]] - vertices[i];
					b = vertices[indices[j - 2]] - vertices[i];
				}

				n = b.crossProduct(a);
				n.normalize();
				normal = normal + faktor * n;

			}
		}

		bezierSurface->pushNormal(normal);
		PointVector tmp = normal * 3;
		normals->pushVertice(vertices[i]);
		normals->pushVertice(vertices[i] + tmp);
		normals->pushColor();
		normals->pushColor();
		normals->pushIndex(k++);
		normals->pushIndex(k++);

	}
}

void Bezier_Surface::subdivideU(float u, float v, vector<Bezier_Surface*>* allSurfaces, bool* lock) {
	*lock = true;


	PolyObject* newPo = new PolyObject(program);
	Bezier_Surface* newSurface = new Bezier_Surface(newPo, deg_m, deg_n, this->t, program);

	vector<CurveBezier*> oldUCurves;
	vector<CurveBezier*> newUCurves;


	for (int i = 0; i < deg_n + 1; i++) {
		PolyObject* controlStr = new PolyObject(program);
		CurveBezier* b = new CurveBezier(controlStr, program);
		if (bezier_toggle) {
			dynamic_cast<Bernstein*>(b);
			b = new Bernstein(controlStr, program);
		}
		else {
			dynamic_cast<DeCasteljau*>(b);
			b = new DeCasteljau(controlStr, program);
		}
		for (int j = 0; j < deg_m + 1; j++) {
			controlStr->pushVertice(controlStructure->getFaces()[i][j]);
			controlStr->pushColor();
		}
		for (int i = 0; i < controlStr->getVertices().size() - 1; i++) {
			controlStructure->pushIndex(i);
			controlStructure->pushIndex(i + 1);
		}
		oldUCurves.push_back(b);
	}

	for (CurveBezier* c : oldUCurves) {
		vector<PointVector> newCurve;
		newCurve.clear();

		c->subdivision(u, newCurve);

		std::reverse(newCurve.begin(), newCurve.end());

		PolyObject* controlPoints = new PolyObject(program);
		controlPoints->setVertices(newCurve);
		for (int i = 0; i < newCurve.size() - 1; i++) {
			controlPoints->pushIndex(i);
			controlPoints->pushIndex(i + 1);
			controlPoints->pushColor();
		}
		PolyObject* po = new PolyObject(program);
		po->setVertices(newCurve);
		CurveBezier* b = new CurveBezier(controlPoints, program);
		if (bezier_toggle) {
			dynamic_cast<Bernstein*>(b);
			b = new Bernstein(controlPoints, program);
		}
		else {
			dynamic_cast<DeCasteljau*>(b);
			b = new DeCasteljau(controlPoints, program);
		}
		b->obj->setVertices(newCurve);
		b->setControlStructure(po);
		b->setInitialized(false);
		newUCurves.push_back(b);
	}

	for (CurveBezier* c : newUCurves) {
		newSurface->u_curves.push_back(c);
	}

	controlStructure->clearVertices();
	for (int i = 0; i < deg_n + 1; i++) {
		for (int j = 0; j < deg_m + 1; j++) {
			newSurface->controlStructure->pushVertice(newUCurves[i]->getControlVertices()[j]);
			newSurface->controlStructure->pushColor();
			controlStructure->pushVertice(oldUCurves[i]->getControlVertices()[j]);
		}
	}
	buildControlStructure();
	updateBezierSurface();
	calcNormals();
	newSurface->buildControlStructure();
	newSurface->updateBezierSurface();
	newSurface->calcNormals();

	PointVector normal = getNormal(u, v);

	subdivideV(v, allSurfaces, true, normal);
	newSurface->subdivideV(v, allSurfaces, false, normal);

	allSurfaces->push_back(newSurface);

	*lock = false;
}

void Bezier_Surface::subdivideV(float v, vector<Bezier_Surface*>* allSurfaces, bool _direction, PointVector& normal) {

	PolyObject* newPo = new PolyObject(program);
	Bezier_Surface* newSurface = new Bezier_Surface(newPo, deg_m, deg_n, this->t, program);

	vector<CurveBezier*> oldVCurves;
	vector<CurveBezier*> newVCurves;


	for (int j = 0; j < deg_m + 1; j++) {
		PolyObject* controlStr = new PolyObject(program);
		CurveBezier* b = new CurveBezier(controlStr, program);
		if (bezier_toggle) {
			dynamic_cast<Bernstein*>(b);
			b = new Bernstein(controlStr, program);
		}
		else {
			dynamic_cast<DeCasteljau*>(b);
			b = new DeCasteljau(controlStr, program);
		}
		for (int i = 0; i < deg_n + 1; i++) {
			controlStr->pushVertice(controlStructure->getFaces()[i][j]);
			controlStr->pushColor();
		}
		for (int i = 0; i < controlStr->getVertices().size() - 1; i++) {
			controlStructure->pushIndex(i);
			controlStructure->pushIndex(i + 1);
		}
		oldVCurves.push_back(b);
	}

	for (CurveBezier* c : oldVCurves) {
		vector<PointVector> newCurve;
		newCurve.clear();

		c->subdivision(v, newCurve);
		std::reverse(newCurve.begin(), newCurve.end());

		PolyObject* controlPoints = new PolyObject(program);
		controlPoints->setVertices(newCurve);
		for (int i = 0; i < newCurve.size() - 1; i++) {
			controlPoints->pushIndex(i);
			controlPoints->pushIndex(i + 1);
			controlPoints->pushColor();
		}
		PolyObject* po = new PolyObject(program);
		po->setVertices(newCurve);
		CurveBezier* b = new CurveBezier(controlPoints, program);
		if (bezier_toggle) {
			dynamic_cast<Bernstein*>(b);
			b = new Bernstein(controlPoints, program);
		}
		else {
			dynamic_cast<DeCasteljau*>(b);
			b = new DeCasteljau(controlPoints, program);
		}
		b->obj->setVertices(newCurve);
		b->setControlStructure(po);
		b->setInitialized(false);
		newVCurves.push_back(b);
	}

	for (CurveBezier* c : newVCurves) {
		newSurface->v_curves.push_back(c);
	}

	controlStructure->clearVertices();
	for (int j = 0; j < deg_n + 1; j++) {
		for (int i = 0; i < deg_m + 1; i++) {
			newSurface->controlStructure->pushVertice(newVCurves[i]->getControlVertices()[j]);
			newSurface->controlStructure->pushColor();
			controlStructure->pushVertice(oldVCurves[i]->getControlVertices()[j]);
		}
	}

	buildControlStructure();
	updateBezierSurface();
	calcNormals();
	newSurface->buildControlStructure();
	newSurface->updateBezierSurface();
	newSurface->calcNormals();


	if (_direction) {
		translate(globalConstants.DRIFT_RANGE * calcDriftVector(1, normal));
		newSurface->translate(globalConstants.DRIFT_RANGE * calcDriftVector(2, normal));
	}
	else {
		translate(globalConstants.DRIFT_RANGE * calcDriftVector(3, normal));
		newSurface->translate(globalConstants.DRIFT_RANGE * calcDriftVector(4, normal));
	}


	allSurfaces->push_back(newSurface);
}

PointVector Bezier_Surface::calcDriftVector(int number, PointVector& normal) {

	PointVector r1(0, -normal.zCoor, normal.yCoor, 0);
	r1.normalize();
	PointVector r2(normal.zCoor, 0, -normal.xCoor, 0);
	r2.normalize();

	switch (number) {
	case 1:
		return 0.5 * r1 + -0.5 * r2;
		break;
	case 2:
		return -0.5 * r1 + -0.5 * r2;
		break;
	case 3:
		return 0.5 * r1 + 0.5 * r2;
		break;
	case 4:
		return -0.5 * r1 + 0.5 * r2;
		break;
	}
}


void Bezier_Surface::setBezierSurfaceProgramNr(unsigned int nr)
{
	bezierSurface->setProgramNr(nr);
}

void Bezier_Surface::setBezier(bool bezier)
{
	bezier_toggle = bezier;
}

void Bezier_Surface::setShowNormals(bool show)
{
	showNormals = show;
}


