#pragma once
#include <stack>
#include "include/GLSLProgram.h"

#include "CurveBezier.h"

class ViewPanel {
public:
	ViewPanel(cg::GLSLProgram*);
	~ViewPanel();
	void init();
	void draw();
	void setView(glm::mat4x4);
	void setProjection(glm::mat4x4);



	void bezierRotX();
	void bezierRotY();
	void bezierRotZ();
	void polyObjRotX();
	void polyObjRotY();
	void polyObjRotZ();

	void zoomIn();
	void zoomOut();

	void addPoint(PointVector point);
	void deletePoint(int position);
	void translate(PointVector direction, int position);

	void selectPoint(glm::vec3&, glm::vec3&);

private:
	std::stack <glm::mat4x4> matrixStack;
	cg::GLSLProgram* program;
	glm::mat4x4 view;
	glm::mat4x4 projection;
	glm::mat4x4 model;

	float zoom = 1.0f;

	CurveBezier* bezier;
	PolyObject* obj;
};