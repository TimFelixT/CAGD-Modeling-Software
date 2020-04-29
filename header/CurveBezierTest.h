#pragma once
#include "include/GLSLProgram.h"

#include "header/PolyObject.h"

class CurveBezierTest {
public:
	CurveBezierTest(PolyObject& pobj, cg::GLSLProgram*);
	~CurveBezierTest();
	void init();
	void draw(glm::mat4x4);


	//Rotationsmethoden
	void rotateX();
	void rotateY();
	void rotateZ();

private:
	PolyObject& obj;

	cg::GLSLProgram* program;
	GLuint vao;
	GLuint positionBuffer;
	GLuint colorBuffer;
	GLuint indexBuffer;

	std::vector<glm::vec3> curveVertices;
	std::vector<GLushort> curveIndices;
	std::vector<glm::vec3> curveColors;


	bool initialized = false;	//Soll aktuell nur einmal gerendert werden, muss ggf. aber angepasst werden, wenn Kontrollpunkte verändert werden

	//Berechnungsmethoden
	void calcCurve();
	int binomialCoefficiant(int, int);
	int factorial(int);

	bool drawControlPoints = false; //TODO

};