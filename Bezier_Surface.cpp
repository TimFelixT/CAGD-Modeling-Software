#include "header/Bezier_Surface.h"
#include "header/ObjFileParser.h"
#include "header/DeCasteljau.h"
#include "header/Bernstein.h"

int lock = 0;

Bezier_Surface::Bezier_Surface(char* filename, cg::GLSLProgram* prog)
{
	t = 2;
	ObjFileParser parser;
	controlStructure = new PolyObject(prog);
	parser.parseObjectFile(filename, controlStructure, &deg_m, &deg_n);
	buildControlStructure();
	calculateBezierSurface();
}

Bezier_Surface::Bezier_Surface(cg::GLSLProgram* prog, vector<CurveBezier*> u_curves_in, vector<CurveBezier*> v_curves_in)
{
	t = 2;
	ObjFileParser parser;
	controlStructure = new PolyObject(prog);
	u_curves = u_curves_in;
	v_curves = v_curves_in;
	buildControlStructure();
	calculateBezierSurface();
}

Bezier_Surface::~Bezier_Surface()
{
}

PolyObject* Bezier_Surface::getBezierSurface()
{
	return bezierSurface;
}

vector<CurveBezier*> Bezier_Surface::getCurves()
{
	return u_curves;
}

void Bezier_Surface::init()
{
	for (CurveBezier* curve : u_curves) {
		curve->init();
		curve->getControlStructure()->init();
		curve->getControlStructure()->setPoints(true);
		curve->getDerativeStructure()->init();
		//curve->getDeCasteljauStructure()->init();
	}
	for (CurveBezier* curve : v_curves) {
		static int c = 0;
		curve->init();
		curve->getControlStructure()->init();
		curve->getControlStructure()->setPoints(true);
		curve->getDerativeStructure()->init();
		c++;
		//curve->getDeCasteljauStructure()->init();
	}
	//calculateBezierSurface();
	bezierSurface->init();
	controlStructure->init();
	controlStructure->setPoints(true);
}

void Bezier_Surface::draw(bool bezier_toggle, glm::mat4x4 projection, glm::mat4x4 view, glm::mat4x4 model) {
	while (lock == 1) { /* wait */ }
	lock = 1;
	controlStructure->draw(projection * view * model);
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
}

void Bezier_Surface::increaseTesselatingRate() {
	if (t < 100) {
		t++;
		bezierSurface->clear();
		calculateBezierSurface();
	}
}

void Bezier_Surface::decreaseTesselatingRate() {
	if (t > 1) {
		t--;
		bezierSurface->clear();
		calculateBezierSurface();
	}
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

	deg_m++;
	calculateVCurves();
}
void Bezier_Surface::degree_increase_v()
{
	for (CurveBezier* curve : v_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			curve->degree_increase();
			//curve->setInitialized(false);
		}
	}


	u_curves.clear();
	deg_n++;
	calculateUCurves();
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
	v_curves.clear();
	while (lock == 1) { /* wait */ }
	lock = 1;

	for (int v = 0; v <= deg_n; v++) {
		vector<PointVector> vcontrol = controlStructure->getFaces().at(v + deg_n + 1);
		PolyObject* v_obj = new PolyObject(controlStructure->getProgram());
		v_obj->setColor(PointVector(1.0f, 0.0f, 1.0f, 0.0f));
		v_obj->setVertices(vcontrol);
		v_obj->pushColor();

		Bernstein* vcurve = new Bernstein(v_obj, v_obj->getProgram());
		vcurve->calcCurve(t);
		vcurve->getControlStructure()->setColor(PointVector(1.0f, 0.0f, 1.0f, 0.0f));
		v_curves.push_back(vcurve);
	}

	lock = 0;
}

void Bezier_Surface::calculateUCurves()
{
	u_curves.clear();
	while (lock == 1) { /* wait */ }
	lock = 1;
	int n = v_curves[0]->getCurveVertices().size();
	for (int k = 0; k < n; k++) {

		vector<PointVector> ucontrol;
		PolyObject* u_obj = new PolyObject(controlStructure->getProgram());
		u_obj->setColor(PointVector(1.0f, 0.0f, 1.0f, 0.0f));

		for (int u = 0; u <= deg_m; u++) {
			ucontrol.push_back(v_curves[u]->getCurveVertices().at(k));
			u_obj->pushVertice(v_curves[u]->getCurveVertices().at(k));
			u_obj->pushColor();
		}

		Bernstein* ucurve = new Bernstein(u_obj, u_obj->getProgram());
		ucurve->calcCurve(t);

		u_curves.push_back(ucurve);

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

	for (int i = 0; i <= deg_n; i++) {
		for (int j = 0; j < deg_m; j++) {
			controlStructure->pushIndex(j * (deg_n +1 )+ i);
			controlStructure->pushIndex((j + 1)* (deg_n + 1) + i);
		}
	}
}

void Bezier_Surface::calculateBezierSurface()
{
	calculateVCurves();
	calculateUCurves();

	v_curves.clear();

	for (int u = 0; u < u_curves[0]->getCurveVertices().size(); u++) {
		vector<PointVector> vcontrol;
		PolyObject* v_obj = new PolyObject(controlStructure->getProgram());
		v_obj->setColor(PointVector(1.0f, 0.0f, 1.0f, 0.0f));
		for (int v = 0; v < u_curves.size(); v++) {
			vcontrol.push_back(u_curves[v]->getCurveVertices().at(u));
			v_obj->pushVertice(u_curves[v]->getCurveVertices().at(u));
			v_obj->pushColor();
		}
		//v_obj->setVertices(vcontrol);
		
		Bernstein* vcurve = new Bernstein(v_obj, v_obj->getProgram());
		vcurve->setCurveVertices(vcontrol);
		v_curves.push_back(vcurve);
	}

	delete bezierSurface;
	bezierSurface = new PolyObject(controlStructure->getProgram());
	bezierSurface->toggleFillSurface();
	bezierSurface->setColor(PointVector(0.0f, 0.0f, 1.0f, 0.0f));

	for (float i = 0; i < u_curves.size(); i++) {
		int n = u_curves[i]->getCurveVertices().size();

		for (float j = 0; j < n - 1; j ++) {
			bezierSurface->pushVertice(u_curves[i]->getCurveVertices().at(j));
			bezierSurface->pushColor();

			if ((n * (i + 1)) + j >= u_curves.size() * v_curves.size()) {
				continue;
			}
			bezierSurface->pushIndex((n * i) + j + 1);
			bezierSurface->pushIndex((n * i) + j);
			bezierSurface->pushIndex((n * (i + 1)) + j);

			bezierSurface->pushIndex((n * i) + j + 1);
			bezierSurface->pushIndex((n * (i + 1)) + j);
			bezierSurface->pushIndex((n * (i + 1)) + j + 1);


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

void Bezier_Surface::subdivisionSurface(float u, float v, vector<CurveBezier*> u_curves1, vector<CurveBezier*> v_curves1, vector<CurveBezier*> u_curves2, vector<CurveBezier*> v_curves2, vector<CurveBezier*> u_curves3, vector<CurveBezier*> v_curves3, vector<CurveBezier*> u_curves4, vector<CurveBezier*> v_curves4) {
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
	tmp3.push_back(v_curves2.at(0)->getControlVertices()[2]);

	subdivision(v, tmp3, tmp4, tmp2);

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
	tmp3.push_back(v_curves2.at(0)->getControlVertices()[1]);

	subdivision(u, tmp3, tmp4, tmp2);
	
	u_curves1[1] = new CurveBezier(tmp4, controlStructure->getProgram());
	u_curves3[1] = new CurveBezier(tmp2, controlStructure->getProgram());
	
	tmp3.clear();
	tmp3.push_back(v_curves3.at(0)->getControlVertices()[1]);
	tmp3.push_back(v_curves3.at(2)->getControlVertices()[1]);
	tmp3.push_back(v_curves4.at(0)->getControlVertices()[1]);

	subdivision(u, tmp3, tmp4, tmp2);

	u_curves2[1] = new CurveBezier(tmp4, controlStructure->getProgram());
	u_curves4[1] = new CurveBezier(tmp2, controlStructure->getProgram());

	tmp3.clear();
	tmp3.push_back(u_curves1.at(0)->getControlVertices()[1]);
	tmp3.push_back(u_curves1.at(2)->getControlVertices()[1]);
	tmp3.push_back(u_curves2.at(0)->getControlVertices()[1]);

	subdivision(v, tmp3, tmp4, tmp2);

	v_curves1[1] = new CurveBezier(tmp4, controlStructure->getProgram());
	v_curves3[1] = new CurveBezier(tmp2, controlStructure->getProgram());

	tmp3.clear();
	tmp3.push_back(u_curves3.at(0)->getControlVertices()[1]);
	tmp3.push_back(u_curves3.at(2)->getControlVertices()[1]);
	tmp3.push_back(u_curves4.at(0)->getControlVertices()[1]);

	subdivision(v, tmp3, tmp4, tmp2);

	v_curves2[1] = new CurveBezier(tmp4, controlStructure->getProgram());
	v_curves4[1] = new CurveBezier(tmp2, controlStructure->getProgram());

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