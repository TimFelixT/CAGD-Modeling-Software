#pragma once

#include "CurveBezier.h"
#include "GlobalFunctions.h"
#include "GlobalConstants.h"
#include "PolyObject.h"

void bezier_subdivision(PolyObject& curve, vector<float> t_list) {
	vector<CurveBezier> curves(t_list.size());
	vector<PointVector> current = curve.getVertices();

	for (auto t : t_list) {
		//PointVector
	}
}

void degree_increase(PolyObject& curve) {
	vector<PointVector> b = curve.getVertices();
	vector<PointVector> b1(b.size() + 1);

	for (int i = 0; i < b.size() + 1; i++) {
		if (i != 0) {
			b1.at(i).xCoor = ((i / b.size()) * b.at(i - 1).xCoor) + (1 - (i / b.size()) * b.at(i).xCoor);
			b1.at(i).yCoor = ((i / b.size()) * b.at(i - 1).yCoor) + (1 - (i / b.size()) * b.at(i).yCoor);
			b1.at(i).zCoor = ((i / b.size()) * b.at(i - 1).zCoor) + (1 - (i / b.size()) * b.at(i).zCoor);
		}
		else {
			b1.at(i).xCoor = b.at(i).xCoor;
			b1.at(i).yCoor = b.at(i).yCoor;
			b1.at(i).zCoor = b.at(i).zCoor;
		}
	}

	curve.setVertices(b1);
}

void rational_bezier(PointVector& point, float new_weight) {
	point.weight = new_weight;

	//CurveBezier neu berechnen
}