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

	//Berechnungsmethoden
	virtual void calcCurve() {};
	virtual void bezier_derivative() {};
	virtual void calcRationalCurve(int w_i, float weight) {};

protected:
	PolyObject* obj;
	PolyObject* d_obj;
	PolyObject* deCasteljauStructure;

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

	friend class Gui;
private:
	unsigned int derivative = 0;
};