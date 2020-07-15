#include "header/DeCasteljau.h"
#include "header/GlobalConstants.h"
#include "header/GlobalFunctions.h"


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

	d_obj->setShowStructurePoints(true);
	d_obj->clear();
	d_obj->clearStructure();

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n - i; j++) {
			controlVertices[j] = (controlVertices[j] * (1 - derivative_t)) + (controlVertices[j + 1] * derivative_t);
			if (i == n - 2 && j == 0) {
				prev_p = controlVertices[j];
			}
			if (i == n - 2 && j == 1) {
				// Vorher gab es zwar die beiden Punkte, jedoch wurde nicht der Punkte an dem die Ableitung tatsächlich ist berechnet
				// Das findet jetzt noch statt. Danach wird wieder der Vektor berechnet, normalisiert und verlängert. 

				p = controlVertices[j];
				prev_p = prev_p * (1 - derivative_t) + (p * derivative_t);
				d_obj->pushVertice(prev_p);
				glm::vec3 d = p.getVec3() - prev_p.getVec3();
				d = glm::normalize(d) * 4.0f;
				p.setVec3((prev_p.getVec3() + d), 1);
				d_obj->pushVertice(p);
				d_obj->pushColor();
				d_obj->pushColor();

				d_obj->addStructurePoint(prev_p.getVec3());
				d_obj->addStructureColor(globalFunctions.mixGlmVector(d_obj->getColor().getVec3()));
				d_obj->addStructurePoint(prev_p.getVec3());
				d_obj->addStructureColor(globalFunctions.mixGlmVector(d_obj->getColor().getVec3()));
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

	for (int w_i = 0; w_i < controlVertices.size(); w_i++) {
		controlVertices.at(w_i) = controlVertices.at(w_i) * controlVertices.at(w_i).weight;
	}

	float t_start;
	float t_end;
	if (t_range_toggle) {
		t_start = -1;
		t_end = 2;
	} else {
		t_start = 0;
		t_end = 1;
	}

	for (;t_start < t_end; t_start += 0.01) {
		p = deCasteljau(controlVertices.size() - 1, 0, t_start, controlVertices);
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


void DeCasteljau::calcRationalCurve(int step)
{
	std::vector<PointVector> controlVertices = obj->getVertices();
	PointVector p;

	curveIndices.clear();
	curveVertices.clear();
	curveColors.clear();
	curveBuffer.clear();

	for (int w_i = 0; w_i < controlVertices.size(); w_i++) {
		controlVertices.at(w_i) = controlVertices.at(w_i) * controlVertices.at(w_i).weight;
	}

	float t_start;
	float t_end;
	if (t_range_toggle) {
		t_start = -1;
		t_end = 2;
	} else {
		t_start = 0;
		t_end = 1;
	}

	for (;t_start <= t_end; t_start += 1.0f / (float)step) {
		p = deCasteljau(controlVertices.size() - 1, 0, t_start, controlVertices);
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

void DeCasteljau::toggleTRange() {
	t_range_toggle = !t_range_toggle;
}