#include "header/ObjFileParser.h"

bool isVertex(string line) {
	return regex_match(line, regex("(\\s*v\\s*(-?\\d+(\\.\\d+)*\\s*){3})\\s*"));
}

bool isNormal(string line) {
	return regex_match(line, regex("(\\s*vn\\s*(-?\\d+(\\.\\d+)*\\s*){3})\\s*"));
}

bool isFace(string line) {
	return regex_match(line, regex("(\\s*f\\s*((\\d+)+/\\d+\\s*)+)\\s*"));
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
		vertice.homoCoor = 0;
	}
	
	return vertice;
}

//PointVector ObjFileParser::parseFaceVertice(char* line) {
//	stringstream stream;
//	const char* delimiter = "/";
//	char* tok;
//	char* next_token;
//	PointVector vertice;
//	unsigned c = 0;
//
//	tok = strtok_s(line, delimiter, &next_token);
//
//	while (tok != 0) {
//		int nr;
//
//		stream << tok;
//		stream >> nr;
//		vertice[c] = nr;
//		c++;
//		if (c == 2) {
//			c = 1;
//		}
//		stream.str("");
//		stream.clear();
//		tok = strtok_s(0, delimiter, &next_token);
//	}
//	return vertice;
//}


void ObjFileParser::parseFace(string line, PolyObject *obj)
{
	smatch m;
	vector<string> vals;

	//Extract all the values
	while (regex_search(line, m, regex("(-?\\d+(\\.\\d+)*)"))) {
		vals.push_back(m.str(1));
		line = m.suffix().str();
	}

	for (auto val : vals) {
		obj->pushIndex(stoi(val));
	}
	/*const char* delimiter = " ";
	char* tok;
	char* next_token;
	vector<PointVector> faces;

	vector<char> cstr(line.c_str(), line.c_str() + line.size() + 1);

	tok = strtok_s(cstr.data(), delimiter, &next_token);

	while (tok != 0) {
		tok = strtok_s(0, delimiter, &next_token);
		if (tok != 0) {
			faces.push_back(parseFaceVertice(tok));
		}
	}
	return faces;*/
}