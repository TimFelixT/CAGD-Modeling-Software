#pragma once

#include <stack>
#include "include/GLSLProgram.h"

#include "Bernstein.h"
#include "Decasteljau.h"
#include "header/Bezier_Surface.h"

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
	void toggleSurface();
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

	void setGui(Gui*);

	bool bezier_toggle = false;
	bool show_surface_toggle = false;
	bool structure_toggle = false;

	void initShader(float, glm::vec4, glm::vec3, glm::vec3, glm::vec3, float);

	bool lockDraw = false;

private:
	std::stack <glm::mat4x4> matrixStack;
	cg::GLSLProgram* program;
	glm::mat4x4 view;
	glm::mat4x4 projection;
	glm::mat4x4 model;


	cg::GLSLProgram programGouraudShaded;
	cg::GLSLProgram programPhongShaded;

	vector<CurveBezier*> allCurves;
	vector<Bezier_Surface*> allSurfaces;

	Bernstein* bernstein_bezier;
	DeCasteljau* deCasteljau_bezier;

	Bezier_Surface* surface;

	PointVector* selectedPointVectorBernstein = nullptr;
	glm::vec3 selectedPointNormalBernstein = glm::vec3(0.0f, 0.0f, 0.0f);
	PointVector* selectedPointVectorDecasteljau = nullptr;
	glm::vec3 selectedPointNormalDecasteljau = glm::vec3(0.0f, 0.0f, 0.0f);
	PointVector* selectedPointVectorSurface = nullptr;
	glm::vec3 selectedPointNormalSurface = glm::vec3(0.0f, 0.0f, 0.0f);
	void selectPointPostCurve(glm::vec3&, glm::vec3&, PolyObject* const&, bool, double&);
	void selectPointPostSurface(glm::vec3&, glm::vec3&, PolyObject* const&, double&);

	void updateBernstein();
	void updateDecasteljau();
	void updateSurface();

	double bernsteinT = 0.5;
	double deCasteljauT = 0.5;



	Gui* gui;
	friend class Gui;


	void setShaderProgram(int);
	void initShader_(cg::GLSLProgram&, const std::string&, const std::string&);
	void initGouraudShader(float, glm::vec4, glm::vec3, glm::vec3, glm::vec3, float);
	void initPhongShader(float, glm::vec4, glm::vec3, glm::vec3, glm::vec3, float);

};