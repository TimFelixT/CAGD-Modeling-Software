#pragma once

#include <stack>
#include "include/GLSLProgram.h"

#include "Bernstein.h"
#include "Decasteljau.h"
#include "../Bezier_Surface.h"

class Gui; //Vermeidet circular dependencies


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

	void polyObjRotX();
	void polyObjRotY();
	void polyObjRotZ();


	void addPoint(PointVector);
	void deletePoint(int);
	void translate(PointVector, int);

	void selectPoint(glm::vec3&, glm::vec3&);
	void dragPoint(glm::vec3&, glm::vec3&);

	void showPoints();

	void drawStructure(double t);
	void drawStructures(vector<double> t);

	void setGui(Gui*);


	bool bezier_toggle = false;

private:
	std::stack <glm::mat4x4> matrixStack;
	cg::GLSLProgram* program;
	glm::mat4x4 view;
	glm::mat4x4 projection;
	glm::mat4x4 model;

	vector<CurveBezier*> allCurves;
	vector<Bezier_Surface*> allSurfaces;

	Gui* gui;

	friend class Gui;
};