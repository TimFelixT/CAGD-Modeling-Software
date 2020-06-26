#pragma once
#include "header/CurveBezier.h"

class Bernstein : public CurveBezier
{
public:
	Bernstein(PolyObject* pobj, cg::GLSLProgram*);
	~Bernstein();
	void bezier_derivative() override;
	void calcCurve() override;
	void calcRationalCurve(int w_i, float weight) override;
private:
	long long binomialCoefficiant(long long, long long);
	long long factorial(long long);

};

