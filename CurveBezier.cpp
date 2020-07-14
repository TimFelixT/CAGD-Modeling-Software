#include "header/CurveBezier.h"
#include "header/GlobalFunctions.h"
#include "header/GlobalConstants.h"



CurveBezier::CurveBezier(PolyObject* pobj, cg::GLSLProgram* prog) : program(prog), obj(pobj) {
	d_obj = new PolyObject(program);
}

CurveBezier::CurveBezier(std::vector<PointVector> verts, cg::GLSLProgram* prog) : program(prog) {
	obj = new PolyObject();
	obj->setVertices(verts);
	d_obj = new PolyObject(program);
}

CurveBezier::~CurveBezier() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &positionBuffer);
}

std::vector<PointVector> CurveBezier::getControlVertices()
{
	return obj->getVertices();
}

PolyObject* CurveBezier::getDeCasteljauStructure()
{
	return deCasteljauStructure;
}
PolyObject* CurveBezier::getControlStructure()
{
	return obj;
}
PolyObject* CurveBezier::getDerativeStructure()
{
	return d_obj;
}

std::vector<PointVector> CurveBezier::getCurveVertices()
{
	return curveVertices;
}

void CurveBezier::setControlStructure(PolyObject* obj)
{
	deCasteljauStructure = obj;
}

void CurveBezier::setCurveVertices(vector<PointVector> verts)
{
	curveVertices = verts;
	int k = 0;
	curveIndices.clear();
	curveBuffer.clear();
	curveColors.clear();
	for (PointVector p : verts) {
		curveIndices.push_back(k);
		curveIndices.push_back(k + 1);
		curveBuffer.push_back(p.getVec3());
		curveColors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		k++;
	}
	curveIndices.pop_back();
}

void CurveBezier::init() {
	if (!initialized) {
		calcRationalCurve();
		bezier_derivative();
		initialized = true;
	}

	GLuint programId = program->getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, curveBuffer.size() * sizeof(glm::vec3), curveBuffer.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, curveColors.size() * sizeof(glm::vec3), curveColors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, curveIndices.size() * sizeof(GLushort), curveIndices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);


}

void CurveBezier::draw(glm::mat4x4 mvp) {
	//derivatives->draw(mvp);

	program->use();
	program->setUniform("mvp", mvp);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_LINES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void CurveBezier::toggleDerivative()
{
	derivative ^= 1;
}

unsigned int CurveBezier::isDerivative()
{
	return derivative;
}


void divide(CurveBezier* bezier, vector<float> t_list) {
	vector<PolyObject> curves(t_list.size() + 1);

}

void CurveBezier::rotateX() {
	globalFunctions.rotateXPointVector(curveVertices);
	updateCurveBuffer();
}
void CurveBezier::rotateY() {
	globalFunctions.rotateYPointVector(curveVertices);
	updateCurveBuffer();
}
void CurveBezier::rotateZ() {
	globalFunctions.rotateZPointVector(curveVertices);
	updateCurveBuffer();
}
void CurveBezier::translate(PointVector v) {
	globalFunctions.translatePointVector(curveVertices, v);
	updateCurveBuffer();
}

void CurveBezier::updateCurveBuffer() {
	curveBuffer.clear();
	for (auto& ptvector : curveVertices) {
		curveBuffer.push_back(ptvector.getVec3());
	}
}
void CurveBezier::setInitialized(bool s) {
	this->initialized = s;
}

void CurveBezier::addPointEnd(PointVector point) {
	obj->pushVertice(point);
	obj->pushColor();
	std::vector<GLushort> indicesobj = obj->getIndices();
	GLushort index = indicesobj.at(indicesobj.size() - 1);
	obj->pushIndex(index);
	obj->pushIndex(index + 1);
	initialized = false;



}
void CurveBezier::deleteLastPoint() {
	std::vector<PointVector> vertices = obj->getVertices();
	vertices.pop_back();
	obj->setVertices(vertices);
	obj->popColor();
	initialized = false;
}

void CurveBezier::subdivision(float t, std::vector<PointVector>& newVertices2) {
	std::vector<PointVector> vertices = obj->getVertices();
	std::vector<PointVector> newVertices;

	for (int i = 0; i < vertices.size() - 1; i++) {
		newVertices.push_back(vertices[0]);
		for (int j = 0; j < vertices.size() - 1 - i; j++) {
			vertices[j] = (vertices[j] * (1 - t)) + (vertices[j + 1] * t);
		}
		newVertices2.push_back(vertices[vertices.size() - 1 - i]);

	}
	newVertices.push_back(vertices[0]);
	newVertices2.push_back(vertices[0]);

	obj->setVertices(newVertices);
}

void CurveBezier::centerCurve() {
	PointVector midPoint(glm::vec3(0.0f, 0.0f, 0.0f), 0);
	vector<PointVector> newPoints;

	for (int i = 0; i < obj->getVertices().size(); i++) {
		midPoint= midPoint + obj->getVertices().at(i);
	}
	float size = obj->getVertices().size();
	midPoint = midPoint * (1.0f/size);
	midPoint = midPoint * (-1);
	midPoint.homoCoor = 0;
	
	obj->translate(midPoint);
	d_obj->translate(midPoint);
	deCasteljauStructure->translate(midPoint);
}