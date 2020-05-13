#ifndef POLY_OBJ
#define POLY_OBJ
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "include/GLSLProgram.h"

#include "PointVector.h"

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
	vector<PointVector> getVertices();
	vector<PointVector> getNormals();
	vector<vector<PointVector>> getFaces();
	std::vector<GLushort> getIndices();
	void setVertices(std::vector<PointVector> in);

	// Push functions
	void pushFace(std::vector<PointVector>);
	void pushVertice(PointVector);
	void pushNormal(PointVector);
	void pushIndex(GLushort index);

	void triangulatePolyNet();

	// Drawing functions
	virtual void init();
	void draw(glm::mat4x4);

	// Rotation functions
	void rotateX();
	void rotateY();
	void rotateZ();

	//Methods to select and move a Point
	void selectPoint(glm::vec3&, glm::vec3&);
	bool dragPoint(glm::vec3&, glm::vec3&);

	void togglePoints();

private:
	vector<vector<PointVector>> triangulatePolygon(vector<PointVector> face);

	vector<PointVector> vertices;
	vector<PointVector> normals;
	vector<PointVector> colors;
	vector<vector<PointVector>> faces;
	std::vector<GLushort> indices;

	cg::GLSLProgram* program;
	GLuint vao;
	GLuint positionBuffer;
	GLuint colorBuffer;
	GLuint indexBuffer;

	vector<glm::vec3> verts;
	vector<glm::vec3> cols;

	void updateCurveBuffer();

	//Needed for moving and selection of a point
	PointVector *selectedPointVector = nullptr;
	glm::vec3 selectedPointNormal = glm::vec3(0.0f,0.0f,0.0f);

	bool showPoints = false;

};

#endif // !POLY_OBJ
