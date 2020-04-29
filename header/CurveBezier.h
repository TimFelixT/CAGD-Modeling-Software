//#pragma once
//#include "include/GLSLProgram.h"
//
//#include "PolyObject.h"
//
//class CurveBezier : public PolyObject {
//public:
//	CurveBezier(cg::GLSLProgram*);
//	~CurveBezier();
//	void init();
//	void draw(glm::mat4x4);
//
//
//	//Rotationsmethoden
//	void rotateX();
//	void rotateY();
//	void rotateZ();
//
//private:
//	cg::GLSLProgram* program;
//	GLuint vao;
//	GLuint positionBuffer;
//	GLuint colorBuffer;
//	GLuint indexBuffer;
//
//
//	std::vector<PointVector> controlVertices = { {0.0f, 5.0f, 0.0f}, { 5.0f, 5.0f, 0.0f }, { 5.0f, -5.0f, 0.0f }, { 0.0f, -5.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { -5.0f, -5.0f, 0.0f } };
//	std::vector<PointVector> colors = { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } };
//	std::vector<GLushort> controlIndices = { 0, 1, 2, 3, 4, 5 };
//
//
//	std::vector<PointVector> curveVertices;
//	std::vector<GLushort> curveIndices;
//	std::vector<PointVector> curveColors;
//
//
//	bool initialized = false;	//Soll aktuell nur einmal gerendert werden, muss ggf. aber angepasst werden, wenn Kontrollpunkte verändert werden
//
//	//Berechnungsmethoden
//	void calcCurve();
//	int binomialCoefficiant(int, int);
//	int factorial(int);
//
//	bool drawControlPoints = false; //TODO
//
//};