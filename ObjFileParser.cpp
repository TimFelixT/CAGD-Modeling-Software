#include "header/ObjFileParser.h"

bool isVertex(string line) {
	return regex_match(line, regex("(\\s*v\\s*(-?\\d+(\\.\\d+)*\\s*){3})\\s*"));
}

bool isNormal(string line) {
	return regex_match(line, regex("(\\s*vn\\s*(-?\\d+(\\.\\d+)*\\s*){3})\\s*"));
}

bool isFace(string line) {
	return regex_match(line, regex("(\\s*f\\s*(\\d+/\\d+/\\d+\\s*)+)\\s*"));
}

bool ObjFileParser::parseObjectFile(const char* filename, PolyObject& polyObj)
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
		parseLine(line, polyObj);
	}

	file.close();
	return true;
}

void ObjFileParser::parseLine(std::string line, PolyObject& polyObj) {
	if (isFace(line)) {
		polyObj.pushFace(parseFace(line));
	}
	else if (isVertex(line)) {
		polyObj.pushVertice(parseVertice(line));
	}
	else if (isNormal(line)) {
		polyObj.pushNormal(parseVertice(line));
	}
}

glm::vec3 ObjFileParser::parseVertice(std::string line) {
	std::smatch m;
	glm::vec3 vertice;
	vector<string> vals;

	//Extract all the values
	while (regex_search(line, m, regex("(-?\\d+(\\.\\d+)*)"))) {
		vals.push_back(m.str(1));
		line = m.suffix().str();
	}

	if (vals.size() == 3) {
		vertice.x = stod(vals.at(0));
		vertice.y = stod(vals.at(1));
		vertice.z = stod(vals.at(2));
	}
	
	return vertice;
}

glm::vec2 ObjFileParser::parseFaceVertice(char* line) {
	stringstream stream;
	const char* delimiter = "/";
	char* tok;
	char* next_token;
	glm::vec2 vertice;
	unsigned c = 0;

	tok = strtok_s(line, delimiter, &next_token);

	while (tok != 0) {
		int nr;

		stream << tok;
		stream >> nr;
		vertice[c] = nr;
		c++;
		if (c == 2) {
			c = 1;
		}
		stream.str("");
		stream.clear();
		tok = strtok_s(0, delimiter, &next_token);
	}
	return vertice;
}


vector<glm::vec2> ObjFileParser::parseFace(string line)
{
	const char* delimiter = " ";
	char* tok;
	char* next_token;
	vector<glm::vec2> faces;

	std::vector<char> cstr(line.c_str(), line.c_str() + line.size() + 1);

	tok = strtok_s(cstr.data(), delimiter, &next_token);

	while (tok != 0) {
		tok = strtok_s(0, delimiter, &next_token);
		if (tok != 0) {
			faces.push_back(parseFaceVertice(tok));
		}
	}
	return faces;
}