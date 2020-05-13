#include "header/ViewPanel.h"
#include <glm/gtc/matrix_transform.hpp>

ViewPanel::ViewPanel(cg::GLSLProgram* prog) : program(prog), model(glm::mat4x4(1.0f)) {
	obj = new PolyObject("testObject.obj", prog);
	bezier = new CurveBezier(*obj, prog);
}

ViewPanel::~ViewPanel() {
	delete obj;
	delete bezier;
}

void ViewPanel::init() {
	obj->init();
	bezier->init();
}

void ViewPanel::draw() {
	matrixStack.push(model);
	matrixStack.push(model);

	bezier->draw(projection * view * model);
	obj->draw(projection * view * model);

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
	obj->rotateX();
}
void ViewPanel::polyObjRotY() {
	obj->rotateY();
}
void ViewPanel::polyObjRotZ() {
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