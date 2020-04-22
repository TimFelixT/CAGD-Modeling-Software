#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>
#include "../include/GLSLProgram.h"
#include "../libs/glm/glm/gtx/rotate_vector.hpp"
#include "GlobalConstants.h"

struct GlobalFunctions {
	
	void rotateXVec3(std::vector<glm::vec3>& vector) {
		for (int i = 0; i < vector.size(); i++) {
			vector[i] = glm::rotateX(vector[i], globalConstants.DEFAULT_ROTATION * ((float)M_PI / 180));
		}
	}
	void rotateYVec3(std::vector<glm::vec3>& vector) {
		for (int i = 0; i < vector.size(); i++) {
			vector[i] = glm::rotateY(vector[i], globalConstants.DEFAULT_ROTATION * ((float)M_PI / 180));
		}
	}
	void rotateZVec3(std::vector<glm::vec3>& vector) {
		for (int i = 0; i < vector.size(); i++) {
			vector[i] = glm::rotateZ(vector[i], globalConstants.DEFAULT_ROTATION * ((float)M_PI / 180));
		}
	}



} globalFunctions;