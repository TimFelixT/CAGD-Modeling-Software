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
	void initGouraudShader(float lightI, glm::vec4 light, glm::vec3 surfKa, glm::vec3 surfKd, glm::vec3 surfKs, float surfShininess);
	void initPhongShader(float lightI, glm::vec4 light, glm::vec3 surfKa, glm::vec3 surfKd, glm::vec3 surfKs, float surfShininess);
	void initShader(float lightI, glm::vec4 light, glm::vec3 surfKa, glm::vec3 surfKd, glm::vec3 surfKs, float surfShininess);
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
	void setFaces(std::vector <std::vector<PointVector>>);
	void setIndices(std::vector<GLushort>);

	// Push functions
	void pushFace(std::vector<PointVector>);
	void pushIFace(std::vector<int>);
	void pushVertice(PointVector);
	void pushColor();
	void pushColor(PointVector);
	void popColor();
	void pushNormal(PointVector);
	void pushIndex(GLushort index);
	void popIndex();

	void clear();
	void clearVertices();
	void clearFaces();
	void clearIndices();

	void clearNormals();

	void triangulatePolyNet();

	// Drawing functions
	virtual void init();
	void draw(glm::mat4x4);

	void draw(glm::mat4x4 projection, glm::mat4x4 view, glm::mat4x4 model, GLenum mode);

	// Rotation functions
	void rotateX();
	void rotateY();
	void rotateZ();
	void translate(PointVector);

	void togglePoints();
	void setPoints(bool);
	void setProgramNr(unsigned int nr);

	//F�r die individuellen Punkte, die nicht alle gleich dem normalen PolyObject sind
	// Keine Indizes n�tig, da Punkte alle einzeln gezeichnet werden
	void addStructureColor(glm::vec3);
	void addStructurePoint(glm::vec3);
	void clearStructure();
	void setShowStructurePoints(bool);

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
	cg::GLSLProgram programGouraudShaded;
	cg::GLSLProgram programPhongShaded;

	GLuint vao;
	GLuint positionBuffer;
	GLuint colorBuffer;
	GLuint indexBuffer;



	GLuint structureVao;
	GLuint structurePositionBuffer;
	GLuint structureColorBuffer;
	vector<glm::vec3> structurePoints;
	vector<glm::vec3> structureColors;
	
	
	vector<glm::vec3> verts;
	vector<glm::vec3> cols;

	void updateCurveBuffer();

	bool showPoints = false;
	unsigned int programNr = 0;
	bool showStructurePoints = false;

	friend class Gui;
	friend class ViewPanel;
};

#endif // !POLY_OBJ
