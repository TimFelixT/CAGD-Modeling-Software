#include "header/CurveBezierTest.h"
#include "header/GlobalFunctions.h"
#include "header/GlobalConstants.h"



CurveBezierTest::CurveBezierTest(PolyObject& pobj, cg::GLSLProgram* prog) : program(prog), obj(pobj) {
}

CurveBezierTest::~CurveBezierTest() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &positionBuffer);
}



void CurveBezierTest::calcCurve() {
	std::vector<PointVector> controlVertices = obj.getVertices();
	float curveStart = controlVertices[0].xCoor;

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
		PointVector point(0.0f, 0.0f, 0.0f, 0.0f);

		for (int i = 0; i <= n; i++)
		{
			point = point + (binomialCoefficiant(n, i) * pow(1 - t, n - i) * pow(t, i)) * controlVertices[i];
		}

		curveVertices.push_back(point);

		currentPoint += globalConstants.BEZIER_ACCURACY;
	}


	for (int i = 0; i < curveVertices.size(); i++)
	{
		if (i != curveVertices.size() - 1) {
			curveIndices.push_back(i);
			curveColors.push_back(PointVector(1.0f, 0.0f, 0.0f, 0.0f ));
			if (i < curveIndices.size())
				curveIndices.push_back(i + 1);
			curveColors.push_back(PointVector(1.0f, 0.0f, 0.0f, 0.0f ));
		}
	}

}

int CurveBezierTest::factorial(int x) {
	if (x > 1)
		return x * factorial(x - 1);
	else
		return 1;
}

int CurveBezierTest::binomialCoefficiant(int n, int k)
{
	return factorial(n) / (factorial(k) * factorial(n - k));
}



void CurveBezierTest::init() {
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
	glBufferData(GL_ARRAY_BUFFER, curveVertices.size() * sizeof(PointVector), curveVertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, curveColors.size() * sizeof(PointVector), curveColors.data(), GL_STATIC_DRAW);

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
void CurveBezierTest::draw(glm::mat4x4 mvp) {

	program->use();
	program->setUniform("mvp", mvp);


	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_LINES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}


void CurveBezierTest::rotateX() {
	vector<glm::vec3> verts;
	for (auto vert : curveVertices) {
		verts.push_back(vert.getVec3());
	}
	globalFunctions.rotateXVec3(verts);

	curveVertices.clear();
	for (auto vert : verts) {
		curveVertices.push_back(PointVector(vert, 0));
	}
}
void CurveBezierTest::rotateY() {
	vector<glm::vec3> verts;
	for (auto vert : curveVertices) {
		verts.push_back(vert.getVec3());
	}
	globalFunctions.rotateYVec3(verts);

	curveVertices.clear();
	for (auto vert : verts) {
		curveVertices.push_back(PointVector(vert, 0));
	}
}
void CurveBezierTest::rotateZ() {
	vector<glm::vec3> verts;
	for (auto vert : curveVertices) {
		verts.push_back(vert.getVec3());
	}
	globalFunctions.rotateZVec3(verts);
	
	curveVertices.clear();
	for (auto vert : verts) {
		curveVertices.push_back(PointVector(vert, 0));
	}
}