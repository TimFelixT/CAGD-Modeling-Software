#pragma once
#include "include/GLSLProgram.h"

#include "header/PolyObject.h"

class CurveBezier {
public:
	CurveBezier(PolyObject* pobj, cg::GLSLProgram*);
	CurveBezier(std::vector<PointVector> verts, cg::GLSLProgram* prog);
	~CurveBezier();
	void init();
	void draw(glm::mat4x4);

	void toggleDerivative();
	unsigned int isDerivative();

	std::vector<PointVector> getControlVertices();
	PolyObject* getDeCasteljauStructure();
	PolyObject* getControlStructure();
	PolyObject* getDerativeStructure();
	std::vector<PointVector> getCurveVertices();

	void setControlStructure(PolyObject* obj);
	void setCurveVertices(vector<PointVector> verts);

	void degree_increase();


	//Rotationsmethoden
	void rotateX();
	void rotateY();
	void rotateZ();


	void addPointEnd(PointVector point);
	void deleteLastPoint();

	void subdivision(float t, std::vector<PointVector>& newVertices2);

	void updateCurveBuffer();
	
	void setInitialized(bool);

	//Berechnungsmethoden
	virtual void calcCurve(int) {};
	virtual void calcCurve() {};
	virtual void bezier_derivative() {};
	virtual void calcRationalCurve() {};
	virtual void calcRationalCurve(int) {};

	virtual vector<PointVector> calcDerivative(float) { return vector<PointVector>(); };

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

	void centerCurve();

	float derivative_t = 0.5;


	bool initialized = false;	//Soll aktuell nur einmal gerendert werden, muss ggf. aber angepasst werden, wenn Kontrollpunkte verändert werden

	friend class Gui;
	friend class ViewPanel;
private:
	unsigned int derivative = 0;
};