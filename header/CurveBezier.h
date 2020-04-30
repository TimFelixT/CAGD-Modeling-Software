#pragma once
#include "include/GLSLProgram.h"

#include "header/PolyObject.h"

class CurveBezier {
public:
	CurveBezier(PolyObject& pobj, cg::GLSLProgram*);
	~CurveBezier();
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

	std::vector<PointVector> curveVertices;
	std::vector<glm::vec3> curveBuffer;

	std::vector<GLushort> curveIndices;
	std::vector<glm::vec3> curveColors;


	bool initialized = false;	//Soll aktuell nur einmal gerendert werden, muss ggf. aber angepasst werden, wenn Kontrollpunkte ver�ndert werden

	//Berechnungsmethoden
	void calcCurve();
	int binomialCoefficiant(int, int);
	int factorial(int);

	bool drawControlPoints = false; //TODO
	void updateCurveBuffer();

};