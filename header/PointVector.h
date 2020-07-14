#pragma once
#include "../include/GLSLProgram.h"

using namespace std;

class PointVector {

public:
	PointVector();												//0, 0, 0, 0
	PointVector(float, float, float, float);					//X, Y, Z, HOMO
	PointVector(glm::vec3&, float);								//VEC3, HOMO
	PointVector(glm::vec4&);									//VEC4
	PointVector(const PointVector&);							//Copy-Konstruktor
	PointVector& operator=(const PointVector&);					//Zuweisungsoperator

	friend PointVector operator*(PointVector&, float);			//Skalarmultiplikation
	friend PointVector operator/(PointVector&, float);			//Skalarmultiplikation
	friend PointVector operator*(float, PointVector&);			//Skalarmultiplikation
	friend PointVector operator/(float, PointVector&);			//Skalarmultiplikation
	friend PointVector operator-(PointVector&, PointVector&);	//Subtraktion
	friend PointVector operator+(PointVector&, PointVector&);	//Addition
	friend float operator*(PointVector&, PointVector&);			//Skalarprodukt
	PointVector crossProduct(PointVector&);						//Kreuzprodukt

	glm::vec3 getVec3();										//GLM Vec3
	glm::vec4 getVec4();										//GLM Vec4

	void setVec3(glm::vec3&, float);
	void setVec4(glm::vec4&);

	float xCoor;
	float yCoor;
	float zCoor;
	float homoCoor;
	float weight = 1;
	void normalize();

	void print();

private:
	void printHomoCoorWarning(string);
};