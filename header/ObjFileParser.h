
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

	bool parseObjectFile(const char* filename, vector<PolyObject*>* polyObj);

	bool parseObjectFile(const char* filename, PolyObject* polyObj, int*, int*, vector<PolyObject*>*, vector<PolyObject*>*);

private:

	void parseLine(string line, PolyObject* polyObj);

	PointVector parseVertice(string line);

	void parseFace(string line, PolyObject*);
};