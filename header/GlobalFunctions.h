#ifndef GLOBAL_FUNCTIONS

#define GLOBAL_FUNCTIONS
#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>
#include "../include/GLSLProgram.h"
#include "../libs/glm/glm/gtx/rotate_vector.hpp"
#include "GlobalConstants.h"

static struct GlobalFunctions {
	
	void rotateXPointVector(std::vector<PointVector>& vector) {
		for (int i = 0; i < vector.size(); i++) {
			vector[i].setVec3(glm::rotateX(vector[i].getVec3(), globalConstants.DEFAULT_ROTATION * ((float)M_PI / 180)), 1);
		}
	}
	void rotateYPointVector(std::vector<PointVector>& vector) {
		for (int i = 0; i < vector.size(); i++) {
			vector[i].setVec3(glm::rotateY(vector[i].getVec3(), globalConstants.DEFAULT_ROTATION * ((float)M_PI / 180)), 1);
		}
	}
	void rotateZPointVector(std::vector<PointVector>& vector) {
		for (int i = 0; i < vector.size(); i++) {
			vector[i].setVec3(glm::rotateZ(vector[i].getVec3(), globalConstants.DEFAULT_ROTATION * ((float)M_PI / 180)), 1);
		}
	}

	void rotateXGlmVector(std::vector<glm::vec3>& vector) {
		for (int i = 0; i < vector.size(); i++) {
			vector[i] = glm::rotateX(vector[i], globalConstants.DEFAULT_ROTATION * ((float)M_PI / 180));
		}
	}
	void rotateYGlmVector(std::vector<glm::vec3>& vector) {
		for (int i = 0; i < vector.size(); i++) {
			vector[i] = glm::rotateY(vector[i], globalConstants.DEFAULT_ROTATION * ((float)M_PI / 180));
		}
	}
	void rotateZGlmVector(std::vector<glm::vec3>& vector) {
		for (int i = 0; i < vector.size(); i++) {
			vector[i] = glm::rotateZ(vector[i], globalConstants.DEFAULT_ROTATION * ((float)M_PI / 180));
		}
	}
	void translatePointVector(std::vector<PointVector>& vector, PointVector trans) {
		for (int i = 0; i < vector.size(); i++) {
			vector[i] = vector[i] + trans;
		}
	}
	void translateGlmVector(std::vector<glm::vec3>& vector, glm::vec3 trans) {
		for (int i = 0; i < vector.size(); i++) {
			vector[i] = vector[i] + trans;
		}
	}


	glm::vec3 mixGlmVector(glm::vec3 v) {
		return glm::vec3(v.z, v.x, v.y);
	}

} globalFunctions;
#endif // !GLOBAL_FUNCTIONS
