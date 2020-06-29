#pragma once
#include "header/CurveBezier.h"
class Bezier_Surface
{
public:
	//Konstruktoren
	Bezier_Surface(char* filename, cg::GLSLProgram*);

	//Destruktor
	~Bezier_Surface();

	vector<PolyObject*> getPolyObjects();
	vector<CurveBezier*> getCurves();
	void pushObject(PolyObject*);

	void init();
	void draw(bool, glm::mat4x4, glm::mat4x4, glm::mat4x4);

	void rationalSurface(int w_i, int w_j, float weight);
	void degree_increase_u();
	void degree_increase_v();
	void divide();

	void rotateX();
	void rotateY();
	void rotateZ();

private:
	void calculateVCurves();
	void calculateUCurves();

	void buildControlStructure();

	void calculateBezierSurface();

	vector<CurveBezier*> u_curves;
	vector<CurveBezier*> v_curves;

	vector<PolyObject*> v_vec;
	vector<PolyObject*> u_vec;

	vector<PolyObject*> objs;
	PolyObject* controlStructure;
	PolyObject* bezierSurface;

	int deg_m, deg_n;
};
