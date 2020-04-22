#pragma once
#include "include/GLSLProgram.h"

class CurveBezier {
public:
	CurveBezier(cg::GLSLProgram*);
	~CurveBezier();
	void init();
	void draw(glm::mat4x4);

private:
	cg::GLSLProgram* program;
	GLuint vao;
	GLuint positionBuffer;
	GLuint colorBuffer;
	GLuint indexBuffer;


};