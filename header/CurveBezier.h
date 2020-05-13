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


	void translate(PointVector direction, int position);
	void addPointEnd(PointVector point);
	void deletePointAt(int position);

	void updateCurveBuffer();
	
	void setInitialized(bool);

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


	bool initialized = false;	//Soll aktuell nur einmal gerendert werden, muss ggf. aber angepasst werden, wenn Kontrollpunkte verändert werden

	//Berechnungsmethoden
	void calcCurve();
	long long binomialCoefficiant(long long, long long);
	long long factorial(long long);

	bool drawControlPoints = false; //TODO
};