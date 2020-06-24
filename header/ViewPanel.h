#pragma once
#include <stack>
#include "include/GLSLProgram.h"

#include "header/Bernstein.h"
#include "header/Decasteljau.h"




class ViewPanel {
public:
	ViewPanel(cg::GLSLProgram*);
	~ViewPanel();
	void init();
	void draw();
	void setView(glm::mat4x4);
	void setProjection(glm::mat4x4);


	void toggleBezierCurve();
	void degreeIncrease();
	void subdivision();
	void derivative();

	void bezierRotX();
	void bezierRotY();
	void bezierRotZ();
	void polyObjRotX();
	void polyObjRotY();
	void polyObjRotZ();


	void addPoint(PointVector point);
	void deletePoint(int position);
	void translate(PointVector direction, int position);

	void selectPoint(glm::vec3&, glm::vec3&);
	void dragPoint(glm::vec3&, glm::vec3&);

	void showPoints();

	void drawStructure(double t);
	void drawStructures(vector<double> t);




	bool bezier_toggle = false;



private:
	std::stack <glm::mat4x4> matrixStack;
	cg::GLSLProgram* program;
	glm::mat4x4 view;
	glm::mat4x4 projection;
	glm::mat4x4 model;

	vector<CurveBezier*> allCurves;

	friend Gui;
};
