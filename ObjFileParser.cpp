#include "header/ObjFileParser.h"
#include "Bezier_Surface.h"

bool isVertex(string line) {
	return regex_match(line, regex("(\\s*v\\s*(-?\\d+(\\.\\d+)*\\s*){3})\\s*"));
}

bool isNormal(string line) {
	return regex_match(line, regex("(\\s*vn\\s*(-?\\d+(\\.\\d+)*\\s*){3})\\s*"));
}

bool isFace(string line) {
	return regex_match(line, regex("(\\s*f\\s*((\\d+)((/\\d+\\s*)*)\\s*)+)"));
}

bool isCurveBezier(string line) {
	return regex_match(line, regex("cstype bezier"));
}

bool isDegree(string line) {
	return regex_match(line, regex("\\s*deg\\s+\\d+\\s+\\d+\\s*"));
}

bool isSurface(string line) {
	return regex_match(line, regex("\\s*surf\\s+(\\d+\\s*)+\\s*"));
}

//
//void parseCurveBezier(fstream* file, vector<PolyObject*>* polyObjs, vector<PointVector>* vertices) {
//	string line;
//
//	while (getline(*file, line))
//	{
//		if (isDegree(line)) {
//			cout << "is degree" << endl;
//		}
//		if (isSurface(line)) {
//			while (line != "end") {
//				PolyObject* obj = new PolyObject();
//				smatch m;
//				vector<char> indices;
//
//				regex face_reg = regex("(\\d+)((/(\\d+))*)");
//				
//				//Extract all the values
//				while (regex_search(line, m, face_reg)) {
//					indices.push_back(m.str(0)[0]);
//					line = m.suffix().str();
//				}
//
//				for (int i = 0; i < indices.size() - 1; i++) {
//					obj->pushVertice(vertices->at((indices[i] - '0') - 1));
//					obj->pushColor(PointVector(0.0f, 1.0f, 1.0f, 1.0f));
//					obj->pushColor(PointVector(0.0f, 1.0f, 1.0f, 1.0f));
//					obj->pushIndex(i);
//					obj->pushIndex(i + 1);
//				}
//				obj->pushVertice(vertices->at((indices[indices.size()-1] - '0') - 1));
//				polyObjs->push_back(obj);
//				getline(*file, line);
//			}
//		}
//	}
//}

void parseCurveBezier(fstream* file, PolyObject* polyObj, int* deg_m, int* deg_n) {
	string line;

	while (getline(*file, line))
	{
		if (isDegree(line)) {
			smatch m;

			regex nr_rgx = regex("\\d+\\s+\\d+");
			regex_search(line, m, nr_rgx);
			*deg_m = (int)(m.str(0)[0] - '0');
			*deg_n = (int)(m.str(0)[m.str(0).size() - 1] - '0');
		}
		if (isSurface(line)) {
			while (line != "end") {
				smatch m;
				vector<char> indices;

				regex face_reg = regex("(\\d+)((/(\\d+))*)");
				
				//Extract all the values
				while (regex_search(line, m, face_reg)) {
					indices.push_back((int)(m.str(0)[0] - '0') - 1);
					line = m.suffix().str();
				}

				for (int i = 0; i < indices.size() - 1; i++) {
					//polyObj->pushVertice(vertices->at((indices[i] - '0') - 1));
					polyObj->pushColor(PointVector(0.0f, 1.0f, 1.0f, 1.0f));
					polyObj->pushColor(PointVector(0.0f, 1.0f, 1.0f, 1.0f));
					polyObj->pushIndex(indices[i]);
					polyObj->pushIndex(indices[i + 1]);
				}
				//polyObj->pushVertice(vertices->at((indices[indices.size()-1] - '0') - 1));
				getline(*file, line);
			}
		}
	}
}

//
//bool ObjFileParser::parseObjectFile(const char* filename, vector<PolyObject*>* polyObjs)
//{
//	fstream file(filename);
//
//	if (!file.good())
//	{
//		string logString = "could not open file \"" + string(filename) + "\"";
//		return false;
//	}
//
//	string line;
//	vector<PointVector> vertices;
//
//	while (getline(file, line))
//	{
//		cout << line << endl;
//		if (isVertex(line)) {
//			vertices.push_back(parseVertice(line));
//		}
//		if (isDegree(line)) {
//
//		}
//		if (isCurveBezier(line)) {
//			parseCurveBezier(&file, polyObjs, &vertices);
//			file.close();
//			return true;
//		}
//	}
//
//	file.close();
//	return true;
//}

bool ObjFileParser::parseObjectFile(const char* filename, PolyObject* polyObj, int* deg_n = 0, int* deg_m = 0)
{
	fstream file(filename);

	if (!file.good())
	{
		string logString = "could not open file \"" + string(filename) + "\"";
		return false;
	}

	string line;

	while (getline(file, line))
	{
		cout << line << endl;
		if (isCurveBezier(line)) {
			parseCurveBezier(&file, polyObj, deg_n, deg_m);
			file.close();
			return true;
		}
		parseLine(line, polyObj);
	}

	file.close();
	return true;
}


void ObjFileParser::parseLine(string line, PolyObject* polyObj) {
	static bool bezier = false;
	if (bezier) {

	}
	else if (isFace(line)) {
		parseFace(line, polyObj);
	}
	else if (isVertex(line)) {
		polyObj->pushVertice(parseVertice(line));
		polyObj->pushColor();
	}
	else if (isNormal(line)) {
		polyObj->pushNormal(parseVertice(line));
	}
	else if (isCurveBezier(line)) {
		bezier = true;
	}
}

PointVector ObjFileParser::parseVertice(string line) {
	smatch m;
	PointVector vertice;
	vector<string> vals;

	//Extract all the values
	while (regex_search(line, m, regex("(-?\\d+(\\.\\d+)*)"))) {
		vals.push_back(m.str(1));
		line = m.suffix().str();
	}

	if (vals.size() == 3) {
		vertice.xCoor = stod(vals.at(0));
		vertice.yCoor = stod(vals.at(1));
		vertice.zCoor = stod(vals.at(2));
		vertice.homoCoor = 1;
	}

	return vertice;
}


void ObjFileParser::parseFace(string line, PolyObject* obj)
{
	smatch m;
	vector<char> verts;
	vector<char> texture_verts;
	vector<char> normal_verts;

	regex face_reg = regex("(-?\\d+)((/(-?\\d+))*)");

	//Extract all the values
	while (regex_search(line, m, face_reg)) {
		verts.push_back(m.str(0)[0]);
		line = m.suffix().str();
	}

	for (int i = 0; i < verts.size() - 1; i++) {
		obj->pushIndex(verts[i] - '0');
		obj->pushIndex(verts[i + 1] - '0');
	}
}