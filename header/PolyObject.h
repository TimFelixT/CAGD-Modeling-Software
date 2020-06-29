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
	PointVector getColor();
	vector<PointVector> getVertices();
	vector<PointVector> getNormals();
	vector<vector<PointVector>> getFaces();
	vector<vector<int>> getIFaces();
	std::vector<GLushort> getIndices();
	cg::GLSLProgram* getProgram();

	void setVertices(std::vector<PointVector> in);
	void setProgram(cg::GLSLProgram*);
	void setColor(PointVector);

	// Push functions
	void pushFace(std::vector<PointVector>);
	void pushIFace(std::vector<int>);
	void pushVertice(PointVector);
	void pushColor();
	void pushColor(PointVector);
	void pushNormal(PointVector);
	void pushIndex(GLushort index);

	void clear();

	void triangulatePolyNet();

	// Drawing functions
	virtual void init();
	void draw(glm::mat4x4);

	void draw(glm::mat4x4 mvp, GLenum mode);

	// Rotation functions
	void rotateX();
	void rotateY();
	void rotateZ();

	//Methods to select and move a Point
	void selectPoint(glm::vec3&, glm::vec3&);
	bool dragPoint(glm::vec3&, glm::vec3&);

	void togglePoints();
	void toggleFillSurface();
	void setPoints(bool);


private:
	vector<vector<PointVector>> triangulatePolygon(vector<PointVector> face);
	PointVector color;


	vector<PointVector> vertices;
	vector<PointVector> normals;
	vector<PointVector> colors;
	vector<vector<PointVector>> faces;
	vector<vector<int>> ifaces;
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
	bool fillSurface = false;

	friend class Gui;
};

#endif // !POLY_OBJ
