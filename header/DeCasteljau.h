#include "header/CurveBezier.h"

class DeCasteljau : public CurveBezier
{
public:
	DeCasteljau(PolyObject* pobj, cg::GLSLProgram*);
	~DeCasteljau();
	void calcCurve() override;
	void calcRationalCurve() override;
	void calcRationalCurve(int) override;
	void bezier_derivative() override;
	void bezier_subdivision(vector<float> t_vec);
	void toggleTRange();

private:
	PointVector deCasteljau(int k, int i, double t0, std::vector<PointVector> P);

	bool t_range_toggle = false;
};

