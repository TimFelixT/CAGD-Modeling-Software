#pragma once
#include "header/CurveBezier.h"
class Bezier_Surface
{
public:
	//Konstruktoren
	Bezier_Surface(char* filename, cg::GLSLProgram*);

	//Destruktor
	~Bezier_Surface();

	PolyObject* getBezierSurface();
	vector<CurveBezier*> getUCurves();
	vector<CurveBezier*> getVCurves();
	vector<CurveBezier*> getCurves();

	void init();
	void draw(bool, glm::mat4x4, glm::mat4x4, glm::mat4x4);

	void increaseTesselatingRate();
	void decreaseTesselatingRate();

	void rationalSurface(int w_i, int w_j, float weight);
	void degree_increase_u();
	void degree_increase_v();
	void divide();

	void rotateX();
	void rotateY();
	void rotateZ();

	void updateCurves();

	void updateBezierSurface();


private:
	void calculateVCurves();
	void calculateUCurves();

	void buildControlStructure();

	void calculateBezierSurface();

	void calcNormals();

	cg::GLSLProgram* program;

	vector<CurveBezier*> u_curves;
	vector<CurveBezier*> v_curves;

	vector<PolyObject*> v_vec;
	vector<PolyObject*> u_vec;

	PolyObject* controlStructure;
	PolyObject* bezierSurface;
	PolyObject* normals;

	int deg_m, deg_n, t;

	friend class Gui;
	friend class ViewPanel;
};

