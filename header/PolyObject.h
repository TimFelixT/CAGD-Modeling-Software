#ifndef POLY_OBJ
#define POLY_OBJ
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "include/GLSLProgram.h"


class PolyObject {
public:

	// Constructors
	PolyObject();
	PolyObject(cg::GLSLProgram*);
	PolyObject(char*, cg::GLSLProgram*);

	//Default copy/move contructor/assignment operator
	PolyObject(const PolyObject&) = default;
	PolyObject(PolyObject&&) = default;
	PolyObject& operator=(const PolyObject&) = default;
	PolyObject& operator=(PolyObject&&) = default;
	~PolyObject();

	// Getters
	std::vector<glm::vec3>& getVertices();
	std::vector<glm::vec3>& getNormals();
	std::vector<std::vector<glm::vec2>>& getFaces();

	// Push functions
	void pushFace(std::vector<glm::vec2>);
	void pushVertice(glm::vec3);
	void pushNormal(glm::vec3);
	void pushIndex(GLushort index);

	void triangulatePolyNet();

	// Drawing functions
	virtual void init();
	void draw(glm::mat4x4);

	// Rotation functions
	void rotateX();
	void rotateY();
	void rotateZ();

private:
	std::vector<std::vector<glm::vec2>> triangulatePolygon(std::vector<glm::vec2> face);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> colors;
	std::vector<std::vector<glm::vec2>> faces;
	std::vector<GLushort> indices;

	cg::GLSLProgram* program;
	GLuint vao;
	GLuint positionBuffer;
	GLuint colorBuffer;
	GLuint indexBuffer;
};

#endif // !POLY_OBJ
