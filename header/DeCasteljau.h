#include "header/CurveBezier.h"

class DeCasteljau : public CurveBezier
{
public:
	DeCasteljau(PolyObject* pobj, cg::GLSLProgram*);
	~DeCasteljau();
	void calcCurve();
	void calcRationalCurve();
	void bezier_derivative();
	void bezier_subdivision(vector<float> t_vec);

private:
	PointVector deCasteljau(int k, int i, double t0, std::vector<PointVector> P);
};

