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

	model = glm::scale(model, glm::vec3(zoom));

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

void ViewPanel::zoomIn() {
	if (zoom < 2.0f) {
		zoom += 0.1f;
	}
}
void ViewPanel::zoomOut() {
	if (zoom > 0.2f) {
		zoom -= 0.1f;
	}
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