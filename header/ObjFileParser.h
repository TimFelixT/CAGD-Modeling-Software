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
	
	bool parseObjectFile(const char* filename, PolyObject* polyObj);

private:

	void parseLine(string line, PolyObject* polyObj);

	PointVector parseVertice(string line);

	PointVector parseFaceVertice(char* line);

	void parseFace(string line, PolyObject*);
};