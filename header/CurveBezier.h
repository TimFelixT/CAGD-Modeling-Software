#pragma once
#include "include/GLSLProgram.h"

#include "header/PolyObject.h"

class CurveBezier {
public:
	CurveBezier(PolyObject* pobj, cg::GLSLProgram*);
	~CurveBezier();

	void init();
	void draw(glm::mat4x4);

	void toggleDerivative();
	unsigned int isDerivative();

	std::vector<PointVector> getControlVertices();
	PolyObject* getDeCasteljauStructure();
	PolyObject* getControlStructure();
	PolyObject* getDerativeStructure();

	void setControlStructure(PolyObject* obj);

	void degree_increase();

	//Rotationsmethoden
	void rotateX();
	void rotateY();
	void rotateZ();


	void translate(PointVector direction, int position);
	void addPointEnd(PointVector point);
	void deletePointAt(int position);

	void updateCurveBuffer();
	
	void setInitialized(bool);

protected:
	PolyObject* obj;
	PolyObject* d_obj;
	PolyObject* deCasteljauStructure;

	cg::GLSLProgram* program;
	GLuint vao;
	GLuint positionBuffer;
	GLuint colorBuffer;
	GLuint indexBuffer;

	std::vector<PointVector> controlVertices;

	std::vector<PointVector> curveVertices;
	std::vector<glm::vec3> curveBuffer;

	std::vector<GLushort> curveIndices;
	std::vector<glm::vec3> curveColors;
	bool initialized = false;	//Soll aktuell nur einmal gerendert werden, muss ggf. aber angepasst werden, wenn Kontrollpunkte verändert werden

	virtual void calcCurve() = 0;
	virtual void bezier_derivative() = 0;
	virtual void calcRationalCurve() = 0;

private:
	unsigned int derivative = 0;
	bool drawControlPoints = false; //TODO
};