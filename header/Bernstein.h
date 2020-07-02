#pragma once
#include "header/CurveBezier.h"

class Bernstein : public CurveBezier
{
public:
	Bernstein(PolyObject* pobj, cg::GLSLProgram*);
	~Bernstein();
	void bezier_derivative() override;
	PointVector calcPoint(vector<PointVector> controlVertices, float t);
	void calcCurve() override;
	void calcCurve(int);
	void calcRationalCurve(int steps) override;
	void calcRationalCurve() override;
private:
	long long binomialCoefficiant(long long, long long);
	long long factorial(long long);


	friend class Bezier_Surface;
};

