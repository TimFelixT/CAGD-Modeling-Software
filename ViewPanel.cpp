#include "header/ViewPanel.h"
#include <glm/gtc/matrix_transform.hpp>

ViewPanel::ViewPanel(cg::GLSLProgram* prog) : program(prog), model(glm::mat4x4(1.0f)) {
	obj = new PolyObject("testObject.obj", prog);
	deCasteljauStructure = new PolyObject(prog);
	bezier = new CurveBezier(*obj, prog);
}

ViewPanel::~ViewPanel() {
	delete deCasteljauStructure;
	delete obj;
	delete bezier;
}

void ViewPanel::init() {
	deCasteljauStructure->init();
	obj->init();
	bezier->init();
}

void ViewPanel::draw() {
	matrixStack.push(model);
	matrixStack.push(model);

	bezier->draw(projection * view * model);
	obj->draw(projection * view * model);
	deCasteljauStructure->draw(projection * view * model);

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
	bezier->addPointEnd(point);
}
void ViewPanel::deletePoint(int position) {
	bezier->deletePointAt(position);
}
void ViewPanel::translate(PointVector direction, int position) {
	bezier->translate(direction, position);
}


void ViewPanel::bezierRotX() {
	bezier->rotateX();
}
void ViewPanel::bezierRotY() {
	bezier->rotateY();
}
void ViewPanel::bezierRotZ() {
	bezier->rotateZ();
}


void ViewPanel::polyObjRotX() {
	deCasteljauStructure->rotateX();
	obj->rotateX();
}
void ViewPanel::polyObjRotY() {
	deCasteljauStructure->rotateY();
	obj->rotateY();
}
void ViewPanel::polyObjRotZ() {
	deCasteljauStructure->rotateZ();
	obj->rotateZ();
}

void ViewPanel::selectPoint(glm::vec3 &cameraPos, glm::vec3 &rayVector) {
	obj->selectPoint(cameraPos, rayVector);
}
void ViewPanel::dragPoint(glm::vec3& cameraPos, glm::vec3& rayVector) {
	if (obj->dragPoint(cameraPos, rayVector)) {
		bezier->updateCurveBuffer();
		bezier->setInitialized(false);
	}
}
void ViewPanel::showPoints() {
	obj->togglePoints();
}

void ViewPanel::drawStructure(double t) {
	std::vector<PointVector>vertices = obj->getVertices();
	std::vector<PointVector>empty;
	deCasteljauStructure->setVertices(empty);

	for (int i = 0; i < vertices.size() - 1; i++) {
		for (int j = 0; j < vertices.size() - 1 - i; j++) {
			vertices[j] = (vertices[j] * (1 - t)) + (vertices[j + 1] * t);
			deCasteljauStructure->pushVertice(vertices[j]);
		}
	}

	int k = 0;

	for (int i = obj->getVertices().size() - 2; i > 0; i--)
	{
		for (int j = 0; j < i; j++) {
			deCasteljauStructure->pushIndex(k);
			deCasteljauStructure->pushIndex(k + 1);
			k++;
		}
		k++;
	}
}