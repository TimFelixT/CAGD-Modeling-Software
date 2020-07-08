#include <math.h>
#include "header/PointVector.h"
using namespace std;


//Konstruktoren
PointVector::PointVector() {
	this->xCoor = 0;
	this->yCoor = 0;
	this->zCoor = 0;
	this->homoCoor = 0;
}
PointVector::PointVector(float x, float y, float z, float homo) {
	this->xCoor = x;
	this->yCoor = y;
	this->zCoor = z;
	this->homoCoor = homo;
	this->weight = 1;
}
PointVector::PointVector(glm::vec3& vec, float homo) {
	this->xCoor = vec.x;
	this->yCoor = vec.y;
	this->zCoor = vec.z;
	this->homoCoor = homo;
	this->weight = 1;
}
PointVector::PointVector(glm::vec4& vec) {
	this->xCoor = vec.x;
	this->yCoor = vec.y;
	this->zCoor = vec.z;
	this->homoCoor = vec.w;
	this->weight = 1;
}

//Copy-Konstruktor
PointVector::PointVector(const PointVector& cp) {
	this->xCoor = cp.xCoor;
	this->yCoor = cp.yCoor;
	this->zCoor = cp.zCoor;
	this->homoCoor = cp.homoCoor;
	this->weight = cp.weight;
}

//Zuweisungsoperator
PointVector& PointVector::operator=(const PointVector& d) {
	if (this != &d) {
		this->xCoor = d.xCoor;
		this->yCoor = d.yCoor;
		this->zCoor = d.zCoor;
		this->homoCoor = d.homoCoor;
		this->weight = d.weight;
	}
	return *this;
}

//Skalarmultiplikation
PointVector operator*(PointVector& ptVector, float scalar) {
	PointVector result;
	result.xCoor = ptVector.xCoor * scalar;
	result.yCoor = ptVector.yCoor * scalar;
	result.zCoor = ptVector.zCoor * scalar;
	result.homoCoor = ptVector.homoCoor * scalar;
	//if (result.homoCoor != 0 && result.homoCoor != 1) result.printHomoCoorWarning("Skalarmultiplikation");
	return result;
}
PointVector operator/(PointVector& ptVector, float scalar){
	PointVector result;
	result.xCoor = ptVector.xCoor / scalar;
	result.yCoor = ptVector.yCoor / scalar;
	result.zCoor = ptVector.zCoor / scalar;
	//result.homoCoor = ptVector.homoCoor / scalar;
	//if (result.homoCoor != 0 && result.homoCoor != 1) result.printHomoCoorWarning("Skalarmultiplikation");
	return result;
}
PointVector operator*(float scalar, PointVector& ptVector) {
	PointVector result;
	result.xCoor = ptVector.xCoor * scalar;
	result.yCoor = ptVector.yCoor * scalar;
	result.zCoor = ptVector.zCoor * scalar;
	result.homoCoor = ptVector.homoCoor * scalar;
	//if (result.homoCoor != 0 && result.homoCoor != 1) result.printHomoCoorWarning("Skalarmultiplikation");
	return result;
}

PointVector operator/(float scalar, PointVector& ptVector){
	PointVector result;
	result.xCoor = ptVector.xCoor / scalar;
	result.yCoor = ptVector.yCoor / scalar;
	result.zCoor = ptVector.zCoor / scalar;
	//result.homoCoor = ptVector.homoCoor / scalar;
	//if (result.homoCoor != 0 && result.homoCoor != 1) result.printHomoCoorWarning("Skalarmultiplikation");
	return result;
}

//Subtraktion
PointVector operator-(PointVector& ptVec1, PointVector& ptVec2) {
	PointVector result;
	result.xCoor = ptVec1.xCoor - ptVec2.xCoor;
	result.yCoor = ptVec1.yCoor - ptVec2.yCoor;
	result.zCoor = ptVec1.zCoor - ptVec2.zCoor;
	result.homoCoor = ptVec1.homoCoor - ptVec2.homoCoor;
	if (result.homoCoor != 0 && result.homoCoor != 1) result.printHomoCoorWarning("Subtraktion");
	return result;
}

//Addition
PointVector operator+(PointVector& ptVec1, PointVector& ptVec2) {
	PointVector result;
	result.xCoor = ptVec1.xCoor + ptVec2.xCoor;
	result.yCoor = ptVec1.yCoor + ptVec2.yCoor;
	result.zCoor = ptVec1.zCoor + ptVec2.zCoor;
	result.homoCoor = ptVec1.homoCoor + ptVec2.homoCoor;
	if (result.homoCoor != 0 && result.homoCoor != 1) result.printHomoCoorWarning("Addition");
	return result;
}

//Skalarprodukt
float operator*(PointVector& ptVec1, PointVector& ptVec2) {
	float result;
	result = (ptVec1.xCoor * ptVec2.xCoor) + (ptVec1.yCoor * ptVec2.yCoor) + (ptVec1.zCoor * ptVec2.zCoor);
	return result;
}

//Kreuzprodukt
PointVector PointVector::crossProduct(PointVector& ptVec2) {
	PointVector result;
	if (this->homoCoor != 0) result.printHomoCoorWarning("Kreuzprodukt Vektor 1");
	if (ptVec2.homoCoor != 0) result.printHomoCoorWarning("Kreuzprodukt Vektor 2");
	result.xCoor = (this->yCoor * ptVec2.zCoor) - (this->zCoor * ptVec2.yCoor);
	result.yCoor = (this->zCoor * ptVec2.xCoor) - (this->xCoor * ptVec2.zCoor);
	result.zCoor = (this->xCoor * ptVec2.yCoor) - (this->yCoor * ptVec2.xCoor);
	result.homoCoor = 0;
	return result;
}

//GLM Vektoren
glm::vec3 PointVector::getVec3() {
	return glm::vec3(this->xCoor, this->yCoor, this->zCoor);
}
glm::vec4 PointVector::getVec4() {
	return glm::vec4(this->xCoor, this->yCoor, this->zCoor, this->homoCoor);
}
void PointVector::setVec3(glm::vec3& vector, float homo) {
	this->xCoor = vector.x;
	this->yCoor = vector.y;
	this->zCoor = vector.z;
	this->homoCoor = homo;
}
void PointVector::setVec4(glm::vec4& vector) {
	this->xCoor = vector.x;
	this->yCoor = vector.y;
	this->zCoor = vector.z;
	this->homoCoor = vector.w;
}



// Warnung wegen homogener Koordinaten
void PointVector::printHomoCoorWarning(string methode) {
	cout << "**********" << endl;
	cout << "Methode: " << methode << endl;
	cout << "Warnung bei homogener Komponenten(" << this->homoCoor << ") der Koordinate X:" << this->xCoor << " Y:" << this->yCoor << " Z:" << this->zCoor << endl;
	cout << "**********" << endl;
}

//Normalisieren
void PointVector::normalize() {
	if(homoCoor == 1){
		//printHomoCoorWarning("Punkt kann nicht normalisiert werden!");
	}
	double length = sqrt(pow(xCoor, 2) + pow(yCoor, 2) + pow(zCoor, 2));
	
	xCoor /= length;
	yCoor /= length;
	zCoor /= length;
}