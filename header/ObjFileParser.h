#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdio.h> 
#include <sstream>
#include <regex>
#include "PolyObject.h"
using namespace std;

class ObjFileParser {
public:
	
	bool parseObjectFile(const char* filename, PolyObject& polyObj);

private:

	void parseLine(string line, PolyObject& polyObj);

	glm::vec3 parseVertice(string line);

	glm::vec2 parseFaceVertice(char* line);

	vector<glm::vec2> parseFace(string line);
};