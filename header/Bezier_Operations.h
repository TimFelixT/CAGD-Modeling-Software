#pragma once

#include "CurveBezier.h"
#include "GlobalFunctions.h"
#include "GlobalConstants.h"
#include "PolyObject.h"


long long factorial(long long x) {
	if (x > 1)
		return x * factorial(x - 1);
	else
		return 1;
}

long long binomialCoefficiant(long long n, long long k)
{
	long long num = factorial(n);
	long long denom = factorial(k) * factorial(n - k);
	if (denom == 0)
		return 0;
	return num / denom;
}

PointVector calcPoint_bernstein(vector<PointVector> controlVertices, float t) {
	int n = controlVertices.size() - 1;
	glm::vec3 point(0.0f, 0.0f, 0.0f);

	for (int i = 0; i <= n; i++)
	{
		point = point + (binomialCoefficiant(n, i) * pow(1 - t, n - i) * pow(t, i)) * controlVertices[i].getVec3();
	}

	PointVector p = PointVector(point, 1);

	return p;
}

PointVector calcDerivative_bernstein(vector<PointVector> controlVertices, float t)
{
	int n = controlVertices.size() - 1;
	glm::vec3 d_point(0.0f, 0.0f, 0.0f);
	glm::vec3 point(0.0f, 0.0f, 0.0f);

	float quot = 0;

	for (int i = 0; i <= n; i++)
	{
		glm::vec3 delta_point;
		if (i < n) {
			delta_point = controlVertices[i + 1].getVec3() - controlVertices[i].getVec3();
			float b1 = binomialCoefficiant(n - 1, i);
			float b2 = pow(1 - t, n - 2 - i);
			float b3;
			if (t == 0) {
				b3 = 1;
			}
			else {
				b3 = pow(t, i - 2);
			}
			d_point = d_point + (b1 * b2 * b3) * delta_point;
		}
		point = point + (binomialCoefficiant(n, i) * pow(1 - t, n - i) * pow(t, i) * controlVertices[i].getVec3());
	}


	glm::vec3 d = ((float)n * d_point) - point;
	d = glm::normalize(d) * 3.0f;
	d_point = point + d;

	return PointVector(d_point, 1);
}