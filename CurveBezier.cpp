#include "header/CurveBezier.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>


CurveBezier::CurveBezier(cg::GLSLProgram* prog) : program(prog) {

}

CurveBezier::~CurveBezier() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &positionBuffer);
}



void CurveBezier::init() {

	//std::vector<glm::vec3> vertices = { {0.0f, 5.0f, 0.0f}, { 5.0f, 5.0f, 0.0f }, { 5.0f, -5.0f, 0.0f }, { 0.0f, -5.0f, 0.0f } };
	//std::vector<glm::vec3> colors = { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } };
	//std::vector<GLushort> indices = { 0, 1, 2, 3 };



}
void CurveBezier::draw(glm::mat4x4 mvp) {


}