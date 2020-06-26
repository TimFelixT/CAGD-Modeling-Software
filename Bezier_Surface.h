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
	void degree_increase();
	void divide();

	void rotateX();
	void rotateY();
	void rotateZ();

private:
	void calculateVCurves();

	void calculateUIndices();

	void calculateVIndices();

	void calculateUCurves();

	vector<CurveBezier*> u_curves;
	vector<CurveBezier*> v_curves;
	vector<PolyObject*> objs;
	PolyObject* obj;

	int deg_m, deg_n;
};

