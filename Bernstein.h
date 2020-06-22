#pragma once
#include "header/CurveBezier.h"

class Bernstein : public CurveBezier
{
public:
	Bernstein(PolyObject* pobj, cg::GLSLProgram*);
	~Bernstein();
	void bezier_derivative();
	void calcCurve();
	void calcRationalCurve();
private:
	long long binomialCoefficiant(long long, long long);
	long long factorial(long long);

};

