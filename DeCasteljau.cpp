#include "header/DeCasteljau.h"


DeCasteljau::DeCasteljau(PolyObject* pobj, cg::GLSLProgram* program) : CurveBezier(pobj, program)
{
	d_obj = new PolyObject(program);

	bezier_derivative();
}

DeCasteljau::~DeCasteljau()
{
}

void DeCasteljau::bezier_derivative() {
	std::vector<PointVector> controlVertices = obj->getVertices();
	PointVector p;
	PointVector prev_p;
	int n = controlVertices.size() - 1;

	d_obj->clear();

	for (double t = 0; t < 1; t += 0.01) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n - i; j++) {
				controlVertices[j] = (controlVertices[j] * (1 - t)) + (controlVertices[j + 1] * t);
				if (i == n - 2 && j == 0) {
					prev_p = controlVertices[j];
				}
				if (i == n - 2 && j == 1) {
					p = controlVertices[j];
					d_obj->pushVertice(prev_p);
					d_obj->pushVertice(p);
					d_obj->pushColor();
					d_obj->pushColor();					
				}
			}
		}
	}


	for (int i = 0; i < d_obj->getVertices().size(); i++)
	{
		d_obj->pushIndex(i);
	}
}


void DeCasteljau::calcCurve() {
	std::vector<PointVector> controlVertices = obj->getVertices();
	PointVector p;

	curveIndices.clear();
	curveVertices.clear();
	curveColors.clear();
	curveBuffer.clear();

	for (double t = 0; t < 1; t += 0.01) {
		p = deCasteljau(controlVertices.size() - 1, 0, t, controlVertices);
		curveVertices.push_back(p);
		curveBuffer.push_back(p.getVec3());
	}


	for (int i = 0; i <= curveVertices.size(); i++)
	{
		if (i != curveVertices.size() - 1) {
			curveIndices.push_back(i);
			curveColors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
			curveIndices.push_back(i + 1);
			curveColors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		}
	}
}

void DeCasteljau::calcRationalCurve()
{
	std::vector<PointVector> controlVertices = obj->getVertices();
	PointVector p;

	curveIndices.clear();
	curveVertices.clear();
	curveColors.clear();
	curveBuffer.clear();

	//if (w_i >= 0 && w_i < controlVertices.size()) {
	//	controlVertices.at(w_i).weight = weight;
	//	controlVertices.at(w_i) = controlVertices.at(w_i) * controlVertices.at(w_i).weight;
	//}

	for (double t = 0; t < 1; t += 0.01) {
		p = deCasteljau(controlVertices.size() - 1, 0, t, controlVertices);
		p.xCoor = p.xCoor / p.homoCoor;
		p.yCoor = p.yCoor / p.homoCoor;
		p.zCoor = p.zCoor / p.homoCoor;
		p.homoCoor = p.homoCoor / p.homoCoor;

		curveVertices.push_back(p);
		curveBuffer.push_back(p.getVec3());
	}


	for (int i = 0; i <= curveVertices.size(); i++)
	{
		if (i != curveVertices.size() - 1) {
			curveIndices.push_back(i);
			curveColors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
			curveIndices.push_back(i + 1);
			curveColors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		}
	}
}

PointVector DeCasteljau::deCasteljau(int k, int i, double t0, std::vector<PointVector> P)
{
	if (k == 0)	return P[i];

	return  deCasteljau(k - 1, i, t0, P) * (1 - t0) + deCasteljau(k - 1, i + 1, t0, P) * t0;
}

void DeCasteljau::bezier_subdivision(vector<float> t_vec) {
	vector<PolyObject> objs;
	vector<PointVector> controlVertices = obj->getVertices();
	vector<vector<PointVector>> new_curves;

	for (auto t : t_vec) {
		vector<PointVector> new_curve;
		for (double i = 0; i < 1; i += t) {
			PointVector p = deCasteljau(controlVertices.size() - 1, 0, i, controlVertices);
			new_curve.push_back(p);
			//if (!(p.yCoor < -1 || p.yCoor > 2)) {
			//curveVertices.push_back(p);
			//curveBuffer.push_back(p.getVec3());
			//}
		}
		new_curves.push_back(new_curve);
	}
}