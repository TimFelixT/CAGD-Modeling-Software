#include "header/Bezier_Surface.h"
#include "header/ObjFileParser.h"
#include "header/DeCasteljau.h"
#include "header/Bernstein.h"

int lock = 0;

Bezier_Surface::Bezier_Surface(char* filename, cg::GLSLProgram* prog)
{
	ObjFileParser parser;
	controlStructure = new PolyObject(prog);
	parser.parseObjectFile(filename, controlStructure, &deg_m, &deg_n, &u_vec, &v_vec);
	buildControlStructure();
	calculateBezierSurface();
	//curves.push_back(new Bernstein(obj, prog));

	//for (auto obj : objs) {
	//	obj->setProgram(prog);
	//	Bernstein* curve = new Bernstein(obj, prog);
	//	//curve->calcCurve();
	//	curves.push_back(curve);
	//}

	//for (int i = 0; i <= deg_n; i++) {
	//	PolyObject* c_obj = new PolyObject(prog);
	//	c_obj->setColor(PointVector(0.0f, 1.0f, 1.0f, 0.0f));
	//	for (int j = 0; j < deg_m; j++) {
	//		c_obj->pushIndex(j);
	//		c_obj->pushIndex(j + 1);
	//		c_obj->pushColor();
	//		c_obj->pushColor();
	//	}
	//	int k = i * deg_m * deg_n;
	//	while (k < (i + 1) * deg_m * deg_n) {
	//		if (k == i * deg_m * deg_n || k % 2 != 0) {
	//			c_obj->pushVertice(obj->getVertices().at(obj->getIndices().at(k)));
	//		}
	//		k++;
	//	}
	//	//c_obj->pushVertice(objs[objs.size() - 1]->getVertices().at(i));

	//	Bernstein* curve = new Bernstein(c_obj, prog);
	//	u_curves.push_back(curve);
	//}
	//calculateVCurves();

	rationalSurface(1, 1, 3);
}

Bezier_Surface::~Bezier_Surface()
{
}

vector<PolyObject*> Bezier_Surface::getPolyObjects()
{
	return objs;
}

vector<CurveBezier*> Bezier_Surface::getCurves()
{
	return u_curves;
}

void Bezier_Surface::pushObject(PolyObject* obj)
{
	objs.push_back(obj);
}

void Bezier_Surface::init()
{
	//obj->init();
	for (CurveBezier* curve : u_curves) {
		curve->init();
		curve->getControlStructure()->init();
		//curve->getControlStructure()->setPoints(true);
		curve->getDerativeStructure()->init();
		//curve->getDeCasteljauStructure()->init();
	}
	for (CurveBezier* curve : v_curves) {
		curve->init();
		curve->getControlStructure()->init();
		//curve->getControlStructure()->setPoints(true);
		curve->getDerativeStructure()->init();
		//curve->getDeCasteljauStructure()->init();
	}
	//calculateBezierSurface();
	bezierSurface->init();
	//controlStructure->init();
}

void Bezier_Surface::draw(bool bezier_toggle, glm::mat4x4 projection, glm::mat4x4 view, glm::mat4x4 model) {
	while (lock == 1) { /* wait */ }
	lock = 1;
	//controlStructure->draw(projection * view * model);
	for (CurveBezier* b : u_curves) {
		//Doppelte Verschachtelung nötig, da sonst nicht alle fälle abgedeckt
		if (bezier_toggle) {
			if (dynamic_cast<Bernstein*>(b)) {
				b->draw(projection * view * model);
				b->getControlStructure()->draw(projection * view * model);
				//b->getDeCasteljauStructure()->draw(projection * view * model);

				if (b->isDerivative() == 1) {
					b->getDerativeStructure()->draw(projection * view * model);
				}
			}
		}
		else if (dynamic_cast<DeCasteljau*>(b)) {
			b->draw(projection * view * model);
			b->getControlStructure()->draw(projection * view * model);
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
				b->draw(projection * view * model);
				b->getControlStructure()->draw(projection * view * model);
				////b->getDeCasteljauStructure()->draw(projection * view * model);

				if (b->isDerivative() == 1) {
					b->getDerativeStructure()->draw(projection * view * model);
				}
			}
		}
		else if (dynamic_cast<DeCasteljau*>(b)) {
			b->draw(projection * view * model);
			b->getControlStructure()->draw(projection * view * model);
			////b->getDeCasteljauStructure()->draw(projection * view * model);

			if (b->isDerivative() == 1) {
				b->getDerativeStructure()->draw(projection * view * model);
			}
		}
	}
	lock = 0;
	bezierSurface->draw(projection * view * model, GL_TRIANGLES);
	//obj->draw(projection * view * model);
}

void Bezier_Surface::rationalSurface(int w_i, int w_j, float weight)
{
	CurveBezier* b = u_curves[w_i];
	PolyObject* structure = b->getControlStructure();
	vector<PointVector> vec = structure->getVertices();
	vec[w_j].weight = weight;
	structure->setVertices(vec);

	CurveBezier* b_v = v_curves[w_j];
	PolyObject* structure_v = b_v->getControlStructure();
	vector<PointVector> vec_v = structure_v->getVertices();
	vec_v[w_i].weight = weight;
	structure_v->setVertices(vec_v);
}

void Bezier_Surface::degree_increase_u()
{
	for (CurveBezier* curve : u_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			curve->degree_increase();
		}
	}

	//v_curves.clear();
	//deg_m++;
	//calculateVCurves();
}
void Bezier_Surface::degree_increase_v()
{
	for (CurveBezier* curve : v_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			curve->degree_increase();
			//curve->setInitialized(false);
		}
	}


	//u_curves.clear();
	//deg_n++;
	//calculateUCurves();
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
			curve->rotateY();
			curve->getControlStructure()->rotateY();
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
			curve->rotateZ();
			curve->getControlStructure()->rotateZ();
			//curve->getDeCasteljauStructure()->rotateZ();
			curve->getDerativeStructure()->rotateZ();
		}
	}
}

void Bezier_Surface::calculateVCurves()
{
	int t = 5;
	while (lock == 1) { /* wait */ }
	lock = 1;

	for (int v = 0; v <= deg_n; v++) {
		vector<PointVector> vcontrol = controlStructure->getFaces().at(v + deg_n + 1);
		PolyObject* v_obj = new PolyObject(controlStructure->getProgram());

		v_obj->setVertices(vcontrol);

		Bernstein* vcurve = new Bernstein(v_obj, v_obj->getProgram());
		vcurve->calcCurve(t);

		v_curves.push_back(vcurve);
	}

	lock = 0;
}

void Bezier_Surface::calculateUCurves()
{
	int t = 5;
	while (lock == 1) { /* wait */ }
	lock = 1;

	for (int k = 0; k <= t; k++) {

		//for (int v = 0; v < deg_n; v++) {
		vector<PointVector> ucontrol;

		for (int u = 0; u <= deg_m; u++) {
			ucontrol.push_back(v_curves[u]->getCurveVertices().at(k));
		}
		PolyObject* u_obj = new PolyObject(controlStructure->getProgram());
		u_obj->setVertices(ucontrol);

		Bernstein* ucurve = new Bernstein(u_obj, u_obj->getProgram());
		ucurve->calcCurve(t);

		u_curves.push_back(ucurve);
		//}

	}
	lock = 0;
}

void Bezier_Surface::buildControlStructure() {
	vector<vector<int>> ifaces = controlStructure->getIFaces();
	vector<vector<PointVector>> faces = controlStructure->getFaces();

	for (int i = 0; i < ifaces.size(); i++) {
		vector<int> new_iface;
		vector<PointVector> new_face;
		for (int j = 0; j < deg_n; j++) {
			new_iface.push_back(ifaces[j].at(i));
			new_face.push_back(faces[j].at(i));
		}
		new_iface.push_back(ifaces[deg_n].at(i));
		new_face.push_back(faces[deg_n].at(i));
		controlStructure->pushIFace(new_iface);
		controlStructure->pushFace(new_face);
	}
	ifaces = controlStructure->getIFaces();

	for (int i = 0; i < ifaces.size(); i++) {
		for (int j = 0; j < deg_n; j++) {
			controlStructure->pushIndex(ifaces[i][j]);
			controlStructure->pushIndex(ifaces[i][j + 1]);
		}
	}
}

void Bezier_Surface::calculateBezierSurface()
{
	int t = 5;

	calculateVCurves();
	calculateUCurves();

	v_curves.clear();

	for (int u = 0; u <= t; u++) {
		vector<PointVector> vcontrol;
		for (int v = 0; v < u_curves.size(); v++) {
			vcontrol.push_back(u_curves[v]->getCurveVertices().at(u));
		}
		PolyObject* v_obj = new PolyObject(controlStructure->getProgram());
		v_obj->setVertices(vcontrol);
		
		Bernstein* vcurve = new Bernstein(v_obj, v_obj->getProgram());
		vcurve->setCurveVertices(vcontrol);
		v_curves.push_back(vcurve);
	}

	bezierSurface = new PolyObject(controlStructure->getProgram());
	bezierSurface->toggleFillSurface();
	bezierSurface->setColor(PointVector(0.0f, 0.0f, 1.0f, 0.0f));

	for (int i = 0; i < u_curves.size(); i++) {
		int n = u_curves[i]->getCurveVertices().size();

		for (int j = 0; j < n - 1; j++) {
			bezierSurface->pushVertice(u_curves[i]->getCurveVertices().at(j));
			bezierSurface->pushColor();

			bezierSurface->pushIndex((n * i) + j);
			bezierSurface->pushIndex((n * i) + j + 1);
			bezierSurface->pushIndex((n * (i + 1)) + j);

			bezierSurface->pushIndex((n * i) + j + 1);
			bezierSurface->pushIndex((n * (i + 1)) + j + 1);
			bezierSurface->pushIndex((n * (i + 1)) + j);
		}
		bezierSurface->pushVertice(u_curves[i]->getCurveVertices().at(n - 1));
		bezierSurface->pushColor();
	}
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