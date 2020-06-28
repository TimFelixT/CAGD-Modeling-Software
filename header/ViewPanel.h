#pragma once

#include <stack>
#include "include/GLSLProgram.h"

#include "Bernstein.h"
#include "Decasteljau.h"


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
	void toggleStructure();
	void degreeIncrease();
	void subdivision(float t, std::vector<PointVector>& newCurveVerts, int curveIndex);
	void derivative();

	void polyObjRotX();
	void polyObjRotY();
	void polyObjRotZ();



	void selectPoint(glm::vec3&, glm::vec3&);

	void dragPoint(glm::vec3&, glm::vec3&);

	void showPoints();

	void drawStructure(int);
	void drawStructures(vector<double> t);

	void setGui(Gui*);


	bool bezier_toggle = false;
	bool structure_toggle = false;

private:
	std::stack <glm::mat4x4> matrixStack;
	cg::GLSLProgram* program;
	glm::mat4x4 view;
	glm::mat4x4 projection;
	glm::mat4x4 model;

	vector<CurveBezier*> allCurves;

	Bernstein* bernstein_bezier;
	DeCasteljau* deCasteljau_bezier;

	PointVector* selectedPointVectorBernstein = nullptr;
	glm::vec3 selectedPointNormalBernstein = glm::vec3(0.0f, 0.0f, 0.0f);
	PointVector* selectedPointVectorDecasteljau = nullptr;
	glm::vec3 selectedPointNormalDecasteljau = glm::vec3(0.0f, 0.0f, 0.0f);
	void selectPointPost(glm::vec3&, glm::vec3&, PolyObject* const&, bool, double&);

	void updateBernstein();
	void updateDecasteljau();

	double bernsteinT = 0.5;
	double deCasteljauT = 0.5;

	Gui* gui;
	friend class Gui;
};