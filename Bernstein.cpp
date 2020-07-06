#include "header/Bernstein.h"
#include "header/GlobalConstants.h"


Bernstein::Bernstein(PolyObject* pobj, cg::GLSLProgram* program) : CurveBezier(pobj, program)
{
	d_obj = new PolyObject(program);
}

Bernstein::~Bernstein()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &positionBuffer);
}

void Bernstein::bezier_derivative() {

	std::vector<PointVector> controlVertices = obj->getVertices();

	d_obj->clear();
	//d_obj->pushVertice(controlVertices[0]);
	d_obj->pushColor();

	int n = controlVertices.size() - 1;

	for (float t = derivative_t; t == derivative_t; t += globalConstants.BEZIER_ACCURACY) {

		glm::vec3 d_point(0.0f, 0.0f, 0.0f);
		glm::vec3 point(0.0f, 0.0f, 0.0f);

		float quot = 0;

		for (int i = 0; i <= n; i++)
		{
			glm::vec3 delta_point;
			if (i < n) {
				delta_point = controlVertices[i + 1].getVec3() - controlVertices[i].getVec3();

				d_point = d_point + (binomialCoefficiant(n-1, i) * pow(1 - t, n - 2 - i) * pow(t, i-2)) * delta_point;
			}
			point = point + (binomialCoefficiant(n, i) * pow(1 - t, n - i) * pow(t, i) * controlVertices[i].getVec3());
		}


		glm::vec3 d = ((float)n*d_point) - point;
		d = glm::normalize(d) * (float)3;
		d_point = point + d;

		d_obj->pushVertice(PointVector(point, 0));
		d_obj->pushVertice(PointVector(d_point, 0));
		d_obj->pushColor();
		d_obj->pushColor();
	}


	for (int i = 0; i < d_obj->getVertices().size() - 1; i++)
	{
		d_obj->pushIndex(i);
		d_obj->pushIndex(i+1);
	}
}

PointVector Bernstein::calcPoint(vector<PointVector> controlVertices, float t) {
		int n = controlVertices.size() - 1;
		glm::vec3 point(0.0f, 0.0f, 0.0f);

		for (int i = 0; i <= n; i++)
		{
			point = point + (binomialCoefficiant(n, i) * pow(1 - t, n - i) * pow(t, i)) * controlVertices[i].getVec3();
		}

		PointVector p = PointVector(point, 0);

		return p;
}

void Bernstein::calcCurve() {
	std::vector<PointVector> controlVertices = obj->getVertices();

	curveIndices.clear();
	curveVertices.clear();
	curveColors.clear();
	curveBuffer.clear();

	float curveLength = 0.0f;

	for (int i = 0; i < controlVertices.size() - 1; i++)
	{
		curveLength += abs(sqrt(pow(controlVertices[i + 1].xCoor - controlVertices[i].xCoor, 2) + pow(controlVertices[i + 1].yCoor - controlVertices[i].yCoor, 2) + pow(controlVertices[i + 1].zCoor - controlVertices[i].zCoor, 2)));
	}


	float currentPoint = 0.0f;
	float currentCurveLength = 0.0f;

	for (float t = 0; t < 1; t += globalConstants.BEZIER_ACCURACY) {
		PointVector p = calcPoint(controlVertices, t);

		curveVertices.push_back(p);
		curveBuffer.push_back(p.getVec3());
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

void Bernstein::calcCurve(int steps) {
	std::vector<PointVector> controlVertices = obj->getVertices();

	curveIndices.clear();
	curveVertices.clear();
	curveColors.clear();
	curveBuffer.clear();

	float curveLength = 0.0f;

	for (int i = 0; i < controlVertices.size() - 1; i++)
	{
		curveLength += abs(sqrt(pow(controlVertices[i + 1].xCoor - controlVertices[i].xCoor, 2) + pow(controlVertices[i + 1].yCoor - controlVertices[i].yCoor, 2) + pow(controlVertices[i + 1].zCoor - controlVertices[i].zCoor, 2)));
	}


	float currentPoint = 0.0f;
	float currentCurveLength = 0.0f;

	int c = 0;
	for (float t = 0; t <= 1; t += (1.0f / (float)steps)) {
		PointVector p = calcPoint(controlVertices, t);

		curveVertices.push_back(p);
		curveBuffer.push_back(p.getVec3());

		if (t + (1.0f / (float)steps) > 1 && c < steps) {
			t = 1 - (1.0f / (float)steps);
		}
		c++;
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

void Bernstein::calcRationalCurve(int steps)
{
	std::vector<PointVector> controlVertices = obj->getVertices();

	curveIndices.clear();
	curveVertices.clear();
	curveColors.clear();
	curveBuffer.clear();

	int c = 0;

	for (float t = 0; t <= 1; t += (1.0f / (float)steps)) {

		int n = controlVertices.size() - 1;
		glm::vec3 point(0.0f, 0.0f, 0.0f);
		//controlVertices[w_i].weight = weight;
		float quot = 0;

		for (int i = 0; i <= n; i++)
		{
			point = point + (controlVertices[i].weight * (binomialCoefficiant(n, i) * pow(1 - t, n - i) * pow(t, i)) * controlVertices[i].getVec3());
			quot += (controlVertices[i].weight * (binomialCoefficiant(n, i) * pow(1 - t, n - i) * pow(t, i)));
		}

		point /= quot;
		PointVector p(point, 0);
		curveVertices.push_back(p);
		curveBuffer.push_back(p.getVec3());


		if (t + (1.0f / (float)steps) > 1 && c < steps) {
			t = 1 - (1.0f / (float)steps);
		}
		c++;
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

void Bernstein::calcRationalCurve()
{
	std::vector<PointVector> controlVertices = obj->getVertices();

	curveIndices.clear();
	curveVertices.clear();
	curveColors.clear();
	curveBuffer.clear();

	for (float t = 0; t < 1; t += globalConstants.BEZIER_ACCURACY) {

		int n = controlVertices.size() - 1;
		glm::vec3 point(0.0f, 0.0f, 0.0f);
		//controlVertices[w_i].weight = weight;
		float quot = 0;

		for (int i = 0; i <= n; i++)
		{
			point = point + (controlVertices[i].weight * (binomialCoefficiant(n, i) * pow(1 - t, n - i) * pow(t, i)) * controlVertices[i].getVec3());
			quot += (controlVertices[i].weight * (binomialCoefficiant(n, i) * pow(1 - t, n - i) * pow(t, i)));
		}

		point /= quot;
		PointVector p(point, 0);
		curveVertices.push_back(p);
		curveBuffer.push_back(p.getVec3());
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

void CurveBezier::degree_increase() {
	vector<PointVector> b = obj->getVertices();
	vector<PointVector> b1(b.size() + 1);
	float n = b.size();

	for (int i = 0; i <= b.size(); i++) {
		if(i < b.size())
			b1.at(i).weight = b.at(i).weight;

		if (i != 0 && i != b.size()) {

			b1.at(i).xCoor = (((float)i / n) * b.at(i - 1).xCoor) + ((1.0f - (i / n)) * b.at(i).xCoor);
			b1.at(i).yCoor = (((float)i / n) * b.at(i - 1).yCoor) + ((1.0f - (i / n)) * b.at(i).yCoor);
			b1.at(i).zCoor = (((float)i / n) * b.at(i - 1).zCoor) + ((1.0f - (i / n)) * b.at(i).zCoor);
		}
		else if (i == 0) {
			b1.at(i).xCoor = b.at(i).xCoor;
			b1.at(i).yCoor = b.at(i).yCoor;
			b1.at(i).zCoor = b.at(i).zCoor;
		}
		else {
			b1.at(i).xCoor = b.at(i - 1).xCoor;
			b1.at(i).yCoor = b.at(i - 1).yCoor;
			b1.at(i).zCoor = b.at(i - 1).zCoor;
		}
	}

	obj->pushIndex(n - 1);
	obj->pushIndex(n);
	obj->pushColor();
	obj->setVertices(b1);

	initialized = false;
}


long long Bernstein::factorial(long long x) {
	if (x > 1)
		return x * factorial(x - 1);
	else
		return 1;
}

long long Bernstein::binomialCoefficiant(long long n, long long k)
{
	long long num = factorial(n);
	long long denom = factorial(k) * factorial(n - k);
	if (denom == 0)
		return 0;
	return num / denom;
}

