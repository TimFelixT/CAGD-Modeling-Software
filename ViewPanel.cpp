#include "header/ViewPanel.h"

ViewPanel::ViewPanel(cg::GLSLProgram* prog) : program(prog), model(glm::mat4x4(1.0f)) {

	bezier = new CurveBezier(prog);
}

ViewPanel::~ViewPanel() {
	delete bezier;
}

void ViewPanel::init() {
	bezier->init();
}

void ViewPanel::draw() {
	bezier->draw(projection * view * model);
}

void ViewPanel::setProjection(glm::mat4x4 p) {
	projection = p;
}
void ViewPanel::setView(glm::mat4x4 v) {
	view = v;
}