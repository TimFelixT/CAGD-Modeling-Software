#include "header/ObjFileParser.h"

bool isVertex(string line) {
	return regex_match(line, regex("(\\s*v\\s*(-?\\d+(\\.\\d+)*\\s*){3})\\s*"));
}

bool isNormal(string line) {
	return regex_match(line, regex("(\\s*vn\\s*(-?\\d+(\\.\\d+)*\\s*){3})\\s*"));
}

bool isFace(string line) {
	return regex_match(line, regex("(\\s*f\\s*((\\d+)((/\\d+\\s*)*)\\s*)+)"));
}

bool ObjFileParser::parseObjectFile(const char* filename, PolyObject* polyObj)
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
		parseLine(line, polyObj);
	}

	file.close();
	return true;
}

void ObjFileParser::parseLine(string line, PolyObject* polyObj) {
	if (isFace(line)) {
		parseFace(line, polyObj);
	}
	else if (isVertex(line)) {
		polyObj->pushVertice(parseVertice(line));
		polyObj->pushColor();
	}
	else if (isNormal(line)) {
		polyObj->pushNormal(parseVertice(line));
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


void ObjFileParser::parseFace(string line, PolyObject *obj)
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