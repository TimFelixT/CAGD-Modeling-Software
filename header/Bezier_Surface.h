#pragma once
#include "header/CurveBezier.h"
class Bezier_Surface
{
public:
	//Konstruktoren
	Bezier_Surface(char* filename, cg::GLSLProgram*);
	Bezier_Surface(PolyObject*, int, int, int, cg::GLSLProgram*);

	Bezier_Surface(cg::GLSLProgram* prog, vector<CurveBezier*> u_curves_in, vector<CurveBezier*> v_curves_in);
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


	void subdivision(float t, std::vector<PointVector>& input, std::vector<PointVector>& newVertices1, std::vector<PointVector>& newVertices2);
	void updateCurves();
	void updateBezierSurface();


	void subdivideU(float, float, vector<Bezier_Surface*>*);
	void subdivideV(float, vector<Bezier_Surface*>*);

	void setBezierSurfaceProgramNr(unsigned int);

private:
	void calculateVCurves();
	void calculateUCurves();

	void buildControlStructure();

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

