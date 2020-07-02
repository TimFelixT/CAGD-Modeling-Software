#include "header/ViewPanel.h"
#include <glm/gtc/matrix_transform.hpp>
#include "header/Gui.h" //Vermeidet circular dependencies
#include "header/GlobalConstants.h"

ViewPanel::ViewPanel(cg::GLSLProgram* prog) : program(prog), model(glm::mat4x4(1.0f)) {

	Bezier_Surface* surface = new Bezier_Surface("UB1_1.obj", prog);

	bernstein_bezier = new Bernstein(new PolyObject("testObject.obj", prog), prog);
	deCasteljau_bezier = new DeCasteljau(new PolyObject("testObject.obj", prog), prog);

	bernstein_bezier->setControlStructure(new PolyObject(program));
	deCasteljau_bezier->setControlStructure(new PolyObject(program));

	allCurves.push_back(bernstein_bezier);
	allCurves.push_back(deCasteljau_bezier);
	allSurfaces.push_back(surface);
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

	for (Bezier_Surface* s : allSurfaces) {
		s->init();
	}
	drawStructure(0); //Bernstein
	drawStructure(1); //DeCasteljau
}

void ViewPanel::setGui(Gui* g) {
	gui = g;
}

void ViewPanel::toggleBezierCurve() {
	bezier_toggle = !bezier_toggle;
}
void ViewPanel::toggleStructure() {
	structure_toggle = !structure_toggle;
}
void ViewPanel::toggleSurface() {
	show_surface_toggle = !show_surface_toggle;
}

void ViewPanel::degreeIncrease() {
	for (CurveBezier* b : allCurves) {
		if (dynamic_cast<Bernstein*>(b)) {
			b->degree_increase();
		}
	}

	for (Bezier_Surface* s : allSurfaces) {
		s->degree_increase_v();
		s->degree_increase_u();
	}
}

void ViewPanel::subdivision(float t, std::vector<PointVector>& newCurveVerts, int curveIndex) {	
	allCurves.at(curveIndex)->subdivision(t, newCurveVerts);
}

void ViewPanel::derivative() {
	for (CurveBezier* b : allCurves) {
		b->toggleDerivative();
	}

	for (Bezier_Surface* s : allSurfaces) {
		for (CurveBezier* b : s->getUCurves()) {
			b->toggleDerivative();
		}
		for (CurveBezier* b : s->getVCurves()) {
			b->toggleDerivative();
		}
	}
}

void ViewPanel::toggleFillSurface() {
	for (Bezier_Surface* s : allSurfaces) {
		s->getBezierSurface()->toggleFillSurface();
	}
}

void ViewPanel::draw() {
	matrixStack.push(model);
	matrixStack.push(model);
	if (show_surface_toggle) {
		for (Bezier_Surface* s : allSurfaces) {
			s->draw(bezier_toggle, projection, view, model);
		}
	} else {
		for (CurveBezier* b : allCurves) {
			//Doppelte Verschachtelung nötig, da sonst nicht alle fälle abgedeckt
			if (bezier_toggle) {
				if (dynamic_cast<Bernstein*>(b)) {
					b->draw(projection * view * model);
					b->getControlStructure()->draw(projection * view * model);

					if (structure_toggle) {
						b->getDeCasteljauStructure()->draw(projection * view * model);
					}

					if (b->isDerivative() == 1) {
						b->getDerativeStructure()->draw(projection * view * model);
					}
				}
			}
			else if (dynamic_cast<DeCasteljau*>(b)) {
				b->draw(projection * view * model);
				b->getControlStructure()->draw(projection * view * model);

				if (structure_toggle) {
					b->getDeCasteljauStructure()->draw(projection * view * model);
				}

				if (b->isDerivative() == 1) {
					b->getDerativeStructure()->draw(projection * view * model);
				}
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


void ViewPanel::polyObjRotX() {
	for (CurveBezier* b : allCurves) {
		if (bezier_toggle) {
			if (dynamic_cast<Bernstein*>(b)) {
				b->rotateX();
				b->getControlStructure()->rotateX();
				b->getDeCasteljauStructure()->rotateX();
				b->getDerativeStructure()->rotateX();
			}
		}
		else if (dynamic_cast<DeCasteljau*>(b)) {
			b->rotateX();
			b->getControlStructure()->rotateX();
			b->getDeCasteljauStructure()->rotateX();
			b->getDerativeStructure()->rotateX();
		}
	}

	for (Bezier_Surface* s : allSurfaces) {
		s->rotateX();
	}
}
void ViewPanel::polyObjRotY() {
	for (CurveBezier* b : allCurves) {
		if (bezier_toggle) {
			if (dynamic_cast<Bernstein*>(b)) {
				b->rotateY();
				b->getControlStructure()->rotateY();
				b->getDeCasteljauStructure()->rotateY();
				b->getDerativeStructure()->rotateY();
			}
		}
		else if (dynamic_cast<DeCasteljau*>(b)) {
			b->rotateY();
			b->getControlStructure()->rotateY();
			b->getDeCasteljauStructure()->rotateY();
			b->getDerativeStructure()->rotateY();
		}
	}

	for (Bezier_Surface* s : allSurfaces) {
		s->rotateY();
	}
}
void ViewPanel::polyObjRotZ() {
	for (CurveBezier* b : allCurves) {
		if (bezier_toggle) {
			if (dynamic_cast<Bernstein*>(b)) {
				b->rotateZ();
				b->getControlStructure()->rotateZ();
				b->getDeCasteljauStructure()->rotateZ();
				b->getDerativeStructure()->rotateZ();
			}
		}
		else if (dynamic_cast<DeCasteljau*>(b)) {
			b->rotateZ();
			b->getControlStructure()->rotateZ();
			b->getDeCasteljauStructure()->rotateZ();
			b->getDerativeStructure()->rotateZ();
		}
	}

	for (Bezier_Surface* s : allSurfaces) {
		s->rotateZ();
	}
}

void ViewPanel::selectPoint(glm::vec3 &cameraPos, glm::vec3 &rayVector) {
	double distance = INFINITY;
	if (show_surface_toggle) {
		for (Bezier_Surface* s : allSurfaces) {
			selectPointPostSurface(cameraPos, rayVector, s->controlStructure, distance);
		}
	}
	else {
		for (CurveBezier* b : allCurves) {
			if (bezier_toggle) {
				if (dynamic_cast<Bernstein*>(b)) {
					selectPointPostCurve(cameraPos, rayVector, b->getControlStructure(), true, distance);
				}
			}
			else if (dynamic_cast<DeCasteljau*>(b)) {
				selectPointPostCurve(cameraPos, rayVector, b->getControlStructure(), false, distance);
			}
		}
	}
}

void ViewPanel::selectPointPostSurface(glm::vec3& cameraPos, glm::vec3& rayVector, PolyObject* const& b, double& distance) {
	for (int i = 0; i < b->vertices.size(); i++) {

		double numerator = glm::length(glm::cross((b->vertices.at(i).getVec3() - cameraPos), rayVector));
		double denumerator = glm::length(rayVector);
		double d = numerator / denumerator;

		if (d < distance) {
			distance = d;
			selectedPointVectorSurface = &(b->vertices.at(i));
		}
	}
	if (distance < globalConstants.SELECTION_OFFSET) {
		selectedPointNormalSurface = rayVector;
		cout << "Selected Surface Point: " << selectedPointVectorSurface->xCoor << "   " << selectedPointVectorSurface->yCoor << "   " << selectedPointVectorSurface->zCoor << endl;
	} else {
		selectedPointVectorSurface = nullptr;
		selectedPointNormalSurface = glm::vec3(0.0f, 0.0f, 0.0f);
		cout << "No point Selected Surface" << endl;
	}
}

void ViewPanel::selectPointPostCurve(glm::vec3& cameraPos, glm::vec3& rayVector, PolyObject* const &b, bool bernstein, double &distance) {
	//For each würde in diesem Fall nicht funktionieren
	for (int i = 0; i < b->vertices.size(); i++) {

		double numerator = glm::length(glm::cross((b->vertices.at(i).getVec3() - cameraPos), rayVector));
		double denumerator = glm::length(rayVector);
		double d = numerator / denumerator;

		if (d < distance) {
			distance = d;
			if (bernstein) {
				selectedPointVectorBernstein = &(b->vertices.at(i));
			} else {
				selectedPointVectorDecasteljau = &(b->vertices.at(i));
			}
		}
	}
	if (distance < globalConstants.SELECTION_OFFSET) {
		if (bernstein) {
			selectedPointNormalBernstein = rayVector;
			cout << "Selected Bernstein Point: " << selectedPointVectorBernstein->xCoor << "   " << selectedPointVectorBernstein->yCoor << "   " << selectedPointVectorBernstein->zCoor << endl;
		} else {
			selectedPointNormalDecasteljau = rayVector;
			cout << "Selected Decasteljau Point: " << selectedPointVectorDecasteljau->xCoor << "   " << selectedPointVectorDecasteljau->yCoor << "   " << selectedPointVectorDecasteljau->zCoor << endl;
		}
	} else {
		if (bernstein) {
			selectedPointVectorBernstein = nullptr;
			selectedPointNormalBernstein = glm::vec3(0.0f, 0.0f, 0.0f);
			cout << "No point Selected Bernstein" << endl;
		} else {
			selectedPointVectorDecasteljau = nullptr;
			selectedPointNormalDecasteljau = glm::vec3(0.0f, 0.0f, 0.0f);
			cout << "No point Selected Decasteljau" << endl;
		}
	}
}



void ViewPanel::dragPoint(glm::vec3& cameraPos, glm::vec3& rayVector) {
	double denominator;

	if (show_surface_toggle) {
		denominator = glm::dot(selectedPointNormalSurface, rayVector);
		if (glm::abs(denominator) > 0.00001) {
			glm::vec3 difference = selectedPointVectorSurface->getVec3() - cameraPos;
			double t = glm::dot(difference, selectedPointNormalSurface) / denominator;
			if (t > 0.0001) {
				glm::vec3 newRay = glm::vec3(t * rayVector.x, t * rayVector.y, t * rayVector.z);
				glm::vec3 newPoint = cameraPos + newRay;
				selectedPointVectorSurface->setVec3(newPoint, 1);
				updateSurface();
			}
		}
	} else {
		if (bezier_toggle) {
			denominator = glm::dot(selectedPointNormalBernstein, rayVector);
			if (glm::abs(denominator) > 0.00001) {
				glm::vec3 difference = selectedPointVectorBernstein->getVec3() - cameraPos;
				double t = glm::dot(difference, selectedPointNormalBernstein) / denominator;

				if (t > 0.0001) {
					glm::vec3 newRay = glm::vec3(t * rayVector.x, t * rayVector.y, t * rayVector.z);
					glm::vec3 newPoint = cameraPos + newRay;
					selectedPointVectorBernstein->setVec3(newPoint, 1);
					updateBernstein();
				}
			}
		} else {
			denominator = glm::dot(selectedPointNormalDecasteljau, rayVector);
			if (glm::abs(denominator) > 0.00001) {
				glm::vec3 difference = selectedPointVectorDecasteljau->getVec3() - cameraPos;
				double t = glm::dot(difference, selectedPointNormalDecasteljau) / denominator;

				if (t > 0.0001) {
					glm::vec3 newRay = glm::vec3(t * rayVector.x, t * rayVector.y, t * rayVector.z);
					glm::vec3 newPoint = cameraPos + newRay;
					selectedPointVectorDecasteljau->setVec3(newPoint, 1);
					updateDecasteljau();
				}
			}
		}
	}
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

void ViewPanel::drawStructure(int curveType) {
	std::vector<PointVector>vertices;
	std::vector<PointVector>empty;
	PolyObject* deCasteljauStructure;

	double t = 0;

	for (CurveBezier* b : allCurves) {
		vertices.clear();
		empty.clear();

		if (curveType == 0) { // 0 = Bernstein, 1 = DeCasteljau
			if (dynamic_cast<Bernstein*>(b)) {
				deCasteljauStructure = b->getDeCasteljauStructure();
				vertices = b->getControlVertices();
				t = bernsteinT;
			}
			else {
				continue;
			}
		}
		else if (dynamic_cast<DeCasteljau*>(b)) {
			deCasteljauStructure = b->getDeCasteljauStructure();
			vertices = b->getControlVertices();
			t = deCasteljauT;
		}
		else {
			continue;
		}

		deCasteljauStructure->clearVertices();

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

void ViewPanel::updateBernstein() {
	for (CurveBezier* b : allCurves) {
		if (dynamic_cast<Bernstein*>(b)) {
			b->setInitialized(false);
		}
	}
}

void ViewPanel::updateDecasteljau() {
	for (CurveBezier* b : allCurves) {
		if (dynamic_cast<DeCasteljau*>(b)) {
			b->setInitialized(false);
		}
	}
}

void ViewPanel::updateSurface() {
	for (Bezier_Surface* s : allSurfaces) {
		s->buildControlStructure();
		s->updateBezierSurface();
	}
}