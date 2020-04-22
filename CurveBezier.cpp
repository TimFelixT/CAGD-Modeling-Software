#include "header/CurveBezier.h"
#include "header/GlobalFunctions.h"



CurveBezier::CurveBezier(cg::GLSLProgram* prog) : program(prog) {
}

CurveBezier::~CurveBezier() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &positionBuffer);
}



void CurveBezier::calcCurve() {

		float curveStart = controlVertices[0].x;

		float curveLength = 0.0f;

		for (int i = 0; i < controlVertices.size() - 1; i++)
		{
			curveLength += abs(sqrt(pow(controlVertices[i + 1].x - controlVertices[i].x, 2) + pow(controlVertices[i + 1].y - controlVertices[i].y, 2) + pow(controlVertices[i + 1].z - controlVertices[i].z, 2) ));
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
				point += (binomialCoefficiant(n, i) * pow(1 - t, n - i) * pow(t, i)) * controlVertices[i];
			}

			curveVertices.push_back(point);

			currentPoint += accuracy;
		}


		for (int i = 0; i < curveVertices.size(); i++)
		{
			if (i != curveVertices.size() - 1) {
				curveIndices.push_back(i);
				curveColors.push_back({ 1.0f, 0.0f, 0.0f });
				if (i < curveIndices.size())
					curveIndices.push_back(i + 1);
					curveColors.push_back({ 1.0f, 0.0f, 0.0f });
			}
		}

}

int CurveBezier::factorial(int x) {
	if (x > 1)
		return x * factorial(x - 1);
	else
		return 1;
}

int CurveBezier::binomialCoefficiant(int n, int k)
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
	glBufferData(GL_ARRAY_BUFFER, curveVertices.size() * sizeof(glm::vec3), curveVertices.data(), GL_STATIC_DRAW);

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


int factorial(int n)
{
	if (n <= 1)
		return(1);
	else
		n = n * factorial(n - 1);
	return n;
}

float binomial_coff(float n, float k)
{
	float ans;
	ans = factorial(n) / (factorial(k) * factorial(n - k));
	return ans;
}

void CurveBezier::rotateX(){
	globalFunctions.rotateXVec3(curveVertices);
}
void CurveBezier::rotateY(){
	globalFunctions.rotateYVec3(curveVertices);
}
void CurveBezier::rotateZ(){
	globalFunctions.rotateZVec3(curveVertices);
}