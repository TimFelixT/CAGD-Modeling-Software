#include "header/PolyObject.h"
#include "header/ObjFileParser.h"
#include "header/GlobalFunctions.h"
#include <algorithm>
#include <limits>
#include <glm\gtc\matrix_inverse.hpp>

//#include "header/GlobalFunctions.h"

PolyObject::PolyObject() {
	color = PointVector();
	color.xCoor = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	color.yCoor = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	color.zCoor = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

PolyObject::PolyObject(cg::GLSLProgram* prog) : program(prog) {
	color = PointVector();

	color.xCoor = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	color.yCoor = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	color.zCoor = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

PolyObject::PolyObject(char* filename, cg::GLSLProgram* prog) : program(prog) {
	color = PointVector();

	color.xCoor = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	color.yCoor = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	color.zCoor = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	ObjFileParser parser;
	parser.parseObjectFile(filename, this, nullptr, nullptr);
}

PolyObject::~PolyObject() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &positionBuffer);
}

void PolyObject::init() {
	this->verts.clear();
	this->cols.clear();
	updateNormalBuffer();

	for (auto vert : vertices) {
		verts.push_back(vert.getVec3());
	}

	for (auto col : colors) {
		cols.push_back(col.getVec3());
	}

	GLuint programId;
	if (programNr == 0) programId = program->getHandle();
	else programId = shaderProgram->getHandle();

	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3), verts.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, cols.size() * sizeof(glm::vec3), cols.data(), GL_STATIC_DRAW);	

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Step 2: Create vertex buffer object for color attribute and bind it to...

	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, norms.size() * sizeof(glm::vec3), norms.data(), GL_STATIC_DRAW);

	// Bind it to normal.
	pos = glGetAttribLocation(programId, "normal");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (normals.size() > 0 && programNr > 0) {
		glGenBuffers(1, &normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, norms.size() * sizeof(glm::vec3), norms.data(), GL_STATIC_DRAW);

		// Bind it to normals.
		pos = glGetAttribLocation(programId, "normal");
		glEnableVertexAttribArray(pos);
		glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	/*Anfang Structure Points*/
	glGenVertexArrays(1, &structureVao);
	glBindVertexArray(structureVao);

	glGenBuffers(1, &structurePositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, structurePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, structurePoints.size() * sizeof(glm::vec3), structurePoints.data(), GL_STATIC_DRAW);

	//pos = glGetAttribLocation(programId, "position");
	//glEnableVertexAttribArray(pos);
	//glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &structureColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, structureColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, structureColors.size() * sizeof(glm::vec3), structureColors.data(), GL_STATIC_DRAW);

	//pos = glGetAttribLocation(programId, "color");
	//glEnableVertexAttribArray(pos);
	//glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//pos = glGetAttribLocation(programId, "normals");
	//glEnableVertexAttribArray(pos);
	//glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Es wird ein Index Buffer gebraucht, kommt aber weil nur Punkte dargestellt werden nie zum Einsatz
	//Statt einen neuen daher zu erstellen, habe ich den schon vorhandenen genommen
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
	/*Ende Structure Points*/

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);
}

void PolyObject::draw(glm::mat4x4 mvp) {

	program->use();
	program->setUniform("mvp", mvp);

	glPointSize(5.0f);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_LINES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	if (showPoints)
		glDrawElements(GL_POINTS, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	if (showStructurePoints && structurePoints.size() > 0) {
		glBindVertexArray(structureVao);
		glDrawElements(GL_POINTS, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	}
	glBindVertexArray(0);
}

void PolyObject::draw(glm::mat4x4 projection, glm::mat4x4 view, glm::mat4x4 model, GLenum mode) {
	glm::mat4 mv = view * model;
	// Create mvp.
	glm::mat4 mvp = projection * mv;

	// Create normal matrix (nm) from model matrix.
	glm::mat3 nm = glm::inverseTranspose(glm::mat3(model));

	if (programNr == 0) {
		program->use();
		program->setUniform("mvp", mvp);
	} else {
		shaderProgram->use();
		shaderProgram->setUniform("modelviewMatrix", mv);
		shaderProgram->setUniform("projectionMatrix", projection);
		shaderProgram->setUniform("normalMatrix", nm);
	}

	glPointSize(5.0f);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	if (programNr > 0) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	glDrawElements(mode, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(0);
}

void PolyObject::rotateX() {
	globalFunctions.rotateXPointVector(vertices);
	globalFunctions.rotateXGlmVector(structurePoints);
	for (vector<PointVector> &v: faces) {
		globalFunctions.rotateXPointVector(v);
	}

	updateCurveBuffer();
}
void PolyObject::rotateY() {
	globalFunctions.rotateYPointVector(vertices);
	globalFunctions.rotateYGlmVector(structurePoints);
	for (vector<PointVector>& v : faces) {
		globalFunctions.rotateYPointVector(v);
	}

	updateCurveBuffer();
}
void PolyObject::rotateZ() {
	globalFunctions.rotateZPointVector(vertices);
	globalFunctions.rotateZGlmVector(structurePoints);
	for (vector<PointVector>& v : faces) {
		globalFunctions.rotateZPointVector(v);
	}

	updateCurveBuffer();
}

void PolyObject::updateCurveBuffer() {
	this->verts.clear();
	for (auto& ptvector : vertices) {
		verts.push_back(ptvector.getVec3());
	}
}

void PolyObject::updateNormalBuffer() {
	this->norms.clear();
	for (auto& ptvector : normals) {
		norms.push_back(ptvector.getVec3());
	}
}

std::vector<std::vector<PointVector>> PolyObject::triangulatePolygon(std::vector<PointVector> face) {
	std::vector<std::vector<PointVector>> new_faces;
	std::vector<PointVector> new_face;

	if (face.size() > 3) {
		for (int i = 1; i < face.size() - 1; i++) {
			new_face.push_back(face.at(0));
			new_face.push_back(face.at(i));
			new_face.push_back(face.at(i + 1));

			new_faces.push_back(new_face);
			new_face.clear();
		}
	}
	else {
		new_faces.push_back(face);
	}
	return new_faces;
}

void PolyObject::triangulatePolyNet() {
	std::vector<std::vector<PointVector>> new_faces;
	std::vector<std::vector<PointVector>> to_append;
	for (auto face : faces) {
		to_append = triangulatePolygon(face);
		new_faces.insert(std::end(new_faces), std::begin(to_append), std::end(to_append));
	}
	faces = new_faces;
}

void PolyObject::pushFace(vector<PointVector> face) {
	faces.push_back(face);
}

void PolyObject::pushIFace(std::vector<int> iface)
{
	ifaces.push_back(iface);
}

void PolyObject::pushIndex(GLushort index) {
	indices.push_back(index);
}
void PolyObject::popIndex() {
	//Zweimal wegen einer Linie
	indices.pop_back();
	indices.pop_back();
}
void PolyObject::clear()
{
	vertices.clear();
	colors.clear();
	indices.clear();
	faces.clear();
	normals.clear();
}

void PolyObject::pushVertice(PointVector vertice) {
	vertices.push_back(vertice);
}

void PolyObject::pushColor()
{
	colors.push_back(color);
}
void PolyObject::popColor() {
	colors.pop_back();
}

void PolyObject::pushColor(PointVector _color)
{
	colors.push_back(_color);
}

void PolyObject::pushNormal(PointVector normal) {
	glm::vec3 norm = glm::normalize(normal.getVec3());

	normals.push_back(PointVector(norm.x, norm.y, norm.z, 0));
}

PointVector PolyObject::getColor()
{
	return color;
}

std::vector<PointVector> PolyObject::getVertices() {
	return vertices;
}

std::vector<PointVector> PolyObject::getNormals() {
	return normals;
}

std::vector<std::vector<PointVector>> PolyObject::getFaces() {
	return faces;
}
vector<vector<int>> PolyObject::getIFaces()
{
	return ifaces;
}
std::vector<GLushort> PolyObject::getIndices() {
	return indices;
}

cg::GLSLProgram* PolyObject::getProgram()
{
	return program;
}

void PolyObject::setVertices(std::vector<PointVector> in) {
	vertices = in;
	indices.clear();
	for (int i = 0; i < in.size() - 1; i++) {
		indices.push_back(i);
		indices.push_back(i + 1);

		colors.push_back(color);
	}
}

void PolyObject::setProgram(cg::GLSLProgram* prog)
{
	program = prog;
}

void PolyObject::setColor(PointVector new_color)
{
	color = new_color;
}

void PolyObject::setFaces(std::vector<std::vector<PointVector>> new_faces)
{
	faces = new_faces;
}

void PolyObject::setIndices(std::vector<GLushort> in)
{
	indices = in;
}

void PolyObject::togglePoints() {
	this->showPoints = !this->showPoints;
}

void PolyObject::setPoints(bool show) {
	showPoints = show;
}

void PolyObject::setProgramNr(unsigned int nr) {
	// 0 -> Wireframe
	// 1 -> Gouraud
	// 2 -> Phong
	if (nr == 0 || nr == 1 || nr == 2)
		programNr = nr;
}

void PolyObject::translate(PointVector tv) {
	for (PointVector& pt : vertices) {
		pt = pt + tv;
	}
}

void PolyObject::clearVertices() {
	vertices.clear();
}

void PolyObject::clearFaces() {
	faces.clear();
}

void PolyObject::clearIndices() {
	indices.clear();
}

void PolyObject::clearNormals() {
	normals.clear();
}

void PolyObject::addStructureColor(glm::vec3 c) {
	this->structureColors.push_back(c);
}
void PolyObject::addStructurePoint(glm::vec3 p) {
	this->structurePoints.push_back(p);
}
void PolyObject::clearStructure() {
	structureColors.clear();
	structurePoints.clear();
}
void PolyObject::setShowStructurePoints(bool b) {
	showStructurePoints = b;
}

void PolyObject::setShaderProgram(cg::GLSLProgram* prog) {
	this->shaderProgram = prog;
}