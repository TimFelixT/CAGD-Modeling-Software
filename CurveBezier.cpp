#include "header/CurveBezier.h"
#include "header/GlobalFunctions.h"
#include "header/GlobalConstants.h"



CurveBezier::CurveBezier(PolyObject& pobj, cg::GLSLProgram* prog) : program(prog), obj(pobj) {
}

CurveBezier::~CurveBezier() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &positionBuffer);
}



void CurveBezier::calcCurve() {
	std::vector<PointVector> controlVertices = obj.getVertices();

	curveIndices.clear();
	curveVertices.clear();
	curveColors.clear();
	curveBuffer.clear();

	float curveLength = 0.0f;

	for (int i = 0; i < controlVertices.size() - 1; i++)
	{
		curveLength += abs(sqrt(pow(controlVertices[i + 1].xCoor - controlVertices[i].xCoor, 2) + pow(controlVertices[i + 1].yCoor 
			- controlVertices[i].yCoor, 2) + pow(controlVertices[i + 1].zCoor - controlVertices[i].zCoor, 2)));
	}


	float currentPoint = 0.0f;
	float currentCurveLength = 0.0f;

	while (currentPoint < curveLength)
	{
		float t = currentPoint / curveLength;

		int n = controlVertices.size() - 1;
		glm::vec3 point(0.0f, 0.0f, 0.0f);

		for (int i = 0; i <= n; i++)
		{
			point = point + (binomialCoefficiant(n, i) * pow(1 - t, n - i) * pow(t, i)) * controlVertices[i].getVec3();
		}

		PointVector p(point, 0);
		curveVertices.push_back(p);
		curveBuffer.push_back(p.getVec3());

		currentPoint += globalConstants.BEZIER_ACCURACY;
	}


	for (int i = 0; i < curveVertices.size(); i++)
	{
		if (i != curveVertices.size() - 1) {
			curveIndices.push_back(i);
			curveColors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
			if (i < curveIndices.size())
				curveIndices.push_back(i + 1);
			curveColors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		}
	}

}

long long CurveBezier::factorial(long long x) {
	if (x > 1)
		return x * factorial(x - 1);
	else
		return 1;
}

long long CurveBezier::binomialCoefficiant(long long n, long long k)
{
	return factorial(n) / (factorial(k) * factorial(n - k));
}



void CurveBezier::init() {
	if (!initialized) {
		calcCurve();
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

	program->use();
	program->setUniform("mvp", mvp);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_LINES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
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
void CurveBezier::updateCurveBuffer() {
	curveBuffer.clear();
	for (auto& ptvector : curveVertices) {
		curveBuffer.push_back(ptvector.getVec3());
	}
}

void CurveBezier::translate(PointVector direction, int position) {
	std::vector<PointVector> vertices = obj.getVertices();
	vertices.at(position) = vertices.at(position) + direction;
	obj.setVertices(vertices);
	initialized = false;
}
void CurveBezier::addPointEnd(PointVector point) {
	obj.pushVertice(point);
	std::vector<GLushort> indicesobj = obj.getIndices();
	GLushort index = indicesobj.at(indicesobj.size() - 1);
	obj.pushIndex(index);
	obj.pushIndex(index + 1);
	initialized = false;
}
void CurveBezier::deletePointAt(int position) {
	std::vector<PointVector> vertices = obj.getVertices();
	if (vertices.size() <= position || position < 0) return;
	vertices.erase(vertices.cbegin() + position);
	obj.setVertices(vertices);
	initialized = false;
}