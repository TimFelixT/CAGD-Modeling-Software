#include "header/ViewPanel.h"
#include <glm/gtc/matrix_transform.hpp>

ViewPanel::ViewPanel(cg::GLSLProgram* prog) : program(prog), model(glm::mat4x4(1.0f)) {
	Bernstein *bernstein_bezier = new Bernstein(new PolyObject("UB1_1.obj", prog), prog);
	DeCasteljau *deCasteljau_bezier = new DeCasteljau(new PolyObject("UB1_1.obj", prog), prog);


	bernstein_bezier->setControlStructure(new PolyObject(program));
	deCasteljau_bezier->setControlStructure(new PolyObject(program));

	allCurves.push_back(bernstein_bezier);
	allCurves.push_back(deCasteljau_bezier);
}

ViewPanel::~ViewPanel() {}

void ViewPanel::init() {
	for (CurveBezier *b : allCurves) {
		b->init();
		b->getControlStructure()->init();
		b->getControlStructure()->setPoints(true);
		b->getDerativeStructure()->init();
		b->getDeCasteljauStructure()->init();
	}
}

void ViewPanel::toggleBezierCurve() {
	bezier_toggle = !bezier_toggle;
}

void ViewPanel::degreeIncrease() {
	for (CurveBezier* b : allCurves) {
		if (dynamic_cast<Bernstein*>(b)) {
			b->degree_increase();
		}
	}
}

void ViewPanel::subdivision() {
	vector<float> t_vec;
	t_vec.push_back(0.25f);
	t_vec.push_back(0.75f);

	//bernstein_bezier->bezier_subdivision(t_vec);
}

void ViewPanel::derivative() {
	for (CurveBezier* b : allCurves) {
		b->toggleDerivative();
	}
}

void ViewPanel::draw() {
	matrixStack.push(model);
	matrixStack.push(model);

	for (CurveBezier* b : allCurves) {
		//Doppelte Verschachtelung n�tig, da sonst nicht alle f�lle abgedeckt
		if (bezier_toggle) {
			if (dynamic_cast<Bernstein*>(b)) {
				b->draw(projection * view * model);
					b->getControlStructure()->draw(projection * view * model);
					b->getDeCasteljauStructure()->draw(projection * view * model);

					if (b->isDerivative() == 1) {
						b->getDerativeStructure()->draw(projection * view * model);
					}
			}
		} else if(dynamic_cast<DeCasteljau*>(b)) {
			b->draw(projection * view * model);
			b->getControlStructure()->draw(projection * view * model);
			b->getDeCasteljauStructure()->draw(projection * view * model);

			if (b->isDerivative() == 1) {
				b->getDerativeStructure()->draw(projection * view * model);
			}
		}
	}

	model = matrixStack.top();
	matrixStack.pop();
	model = matrixStack.top();
	matrixStack.pop();
}

void ViewPanel::setProjection(glm::mat4x4 p) {
	projection = p;
}
void ViewPanel::setView(glm::mat4x4 v) {
	view = v;
}

void ViewPanel::addPoint(PointVector point) {
	//bernstein_bezier->addPointEnd(point);
}
void ViewPanel::deletePoint(int position) {
	//bernstein_bezier->deletePointAt(position);
}
void ViewPanel::translate(PointVector direction, int position) {
	//bernstein_bezier->translate(direction, position);
}


void ViewPanel::bezierRotX() {
	//bernstein_bezier->rotateX();
}
void ViewPanel::bezierRotY() {
	//bernstein_bezier->rotateY();
}
void ViewPanel::bezierRotZ() {
	//bernstein_bezier->rotateZ();
}


void ViewPanel::polyObjRotX() { /*
	if (bezier_toggle) {
		bernstein_bezier->rotateX();
		bernstein_bezier->getControlStructure()->rotateX();
		bernstein_bezier->getDeCasteljauStructure()->rotateX();
		bernstein_bezier->getDerativeStructure()->rotateX();
	}
	else {
		deCasteljau_bezier->rotateX();
		deCasteljau_bezier->getControlStructure()->rotateX();
		deCasteljau_bezier->getDeCasteljauStructure()->rotateX();
		deCasteljau_bezier->getDerativeStructure()->rotateX();
	}*/
}
void ViewPanel::polyObjRotY() { /*
	if (bezier_toggle) {
		bernstein_bezier->rotateY();
		bernstein_bezier->getControlStructure()->rotateY();
		bernstein_bezier->getDeCasteljauStructure()->rotateY();
		bernstein_bezier->getDerativeStructure()->rotateY();
	}
	else {
		deCasteljau_bezier->rotateY();
		deCasteljau_bezier->getControlStructure()->rotateY();
		deCasteljau_bezier->getDeCasteljauStructure()->rotateY();
		deCasteljau_bezier->getDerativeStructure()->rotateY();
	}*/
}
void ViewPanel::polyObjRotZ() { /*
	if (bezier_toggle) {
		bernstein_bezier->rotateZ();
		bernstein_bezier->getControlStructure()->rotateZ();
		bernstein_bezier->getDeCasteljauStructure()->rotateZ();
		bernstein_bezier->getDerativeStructure()->rotateZ();
	}
	else {
		deCasteljau_bezier->rotateZ();
		deCasteljau_bezier->getControlStructure()->rotateZ();
		deCasteljau_bezier->getDeCasteljauStructure()->rotateZ();
		deCasteljau_bezier->getDerativeStructure()->rotateZ();
	}*/
}

void ViewPanel::selectPoint(glm::vec3 &cameraPos, glm::vec3 &rayVector) {
	/*if (bezier_toggle)
		bernstein_bezier->getControlStructure()->selectPoint(cameraPos, rayVector);
	else
		deCasteljau_bezier->getControlStructure()->selectPoint(cameraPos, rayVector);*/
}
void ViewPanel::dragPoint(glm::vec3& cameraPos, glm::vec3& rayVector) {
	/*if (bezier_toggle) {
		if (bernstein_bezier->getControlStructure()->dragPoint(cameraPos, rayVector)) {
			bernstein_bezier->updateCurveBuffer();
			bernstein_bezier->setInitialized(false);
		}
	}*/
}
void ViewPanel::showPoints() {
	for (CurveBezier* b : allCurves) {
		if (bezier_toggle) {
			if (dynamic_cast<Bernstein*>(b))
				b->getControlStructure()->togglePoints();
		} else if (dynamic_cast<DeCasteljau*>(b)) {
			b->getControlStructure()->togglePoints();
		}
	}
}

void ViewPanel::drawStructure(double t) {
	std::vector<PointVector>vertices;
	std::vector<PointVector>empty;
	PolyObject* deCasteljauStructure;

	for (CurveBezier* b : allCurves) {
		vertices.clear();
		empty.clear();

		if (bezier_toggle) {
			if (dynamic_cast<Bernstein*>(b)) {
				deCasteljauStructure = b->getDeCasteljauStructure();
				vertices = b->getControlVertices();
			} else {
				continue;
			}
		} else if (dynamic_cast<DeCasteljau*>(b)) {
			deCasteljauStructure = b->getDeCasteljauStructure();
			vertices = b->getControlVertices();
		} else {
			continue;
		}

		deCasteljauStructure->setVertices(empty);

		for (int i = 0; i < vertices.size() - 1; i++) {
			for (int j = 0; j < vertices.size() - 1 - i; j++) {
				vertices[j] = (vertices[j] * (1 - t)) + (vertices[j + 1] * t);
				deCasteljauStructure->pushVertice(vertices[j]);
				deCasteljauStructure->pushColor();
			}
		}

		int k = 0;

		for (int i = vertices.size() - 2; i > 0; i--) {
			for (int j = 0; j < i; j++) {
				deCasteljauStructure->pushIndex(k);
				deCasteljauStructure->pushIndex(k + 1);
				k++;
			}
			k++;
		}
	}
}


//void ViewPanel::drawStructures(vector<double> t_vec) {
//	std::vector<PointVector>vertices = obj->getVertices();
//	std::vector<PointVector>empty;
//	PolyObject* deCasteljauStructure = bernstein_bezier->getControlStructure();
//
//	for (int r = 0; r < t_vec.size(); r++) {
//		PolyObject* structure = new PolyObject(program);
//		for (int i = 0; i < vertices.size() - 1; i++) {
//			PointVector vertice;
//			//for (int j = 0; j < vertices.size() - 1 - i; j++) {
//			//	vertices[j] = (vertices[j] * (1 - t_vec.at(r))) + (vertices[j + 1] * t_vec.at(r));
//			//	structure->pushVertice(vertices[j]);
//			//	structure->pushColor();
//			//}
//
//			for (int k = 0; k < vertices.size() - 1; k++) {
//				//vertices[k] = vertices[k] * ();
//				structure->pushVertice(vertices[k]);
//
//			}
//
//		}
//		deCasteljauStructures.push_back(structure);
//		vertices = obj->getVertices();
//	}
//
//	int k = 0;
//
//	for (auto* structure : deCasteljauStructures) {
//		for (int i = obj->getVertices().size() - 2; i > 0; i--)
//		{
//			for (int j = 0; j < i; j++) {
//				structure->pushIndex(k);
//				structure->pushIndex(k + 1);
//				k++;
//			}
//			k++;
//		}
//		k = 0;
//	}
//}