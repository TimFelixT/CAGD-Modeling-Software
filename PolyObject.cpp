#include "header/PolyObject.h"
#include "header/ObjFileParser.h"
#include "header/GlobalFunctions.h"
#include <algorithm>
#include <limits>

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
	parser.parseObjectFile(filename, this, nullptr, nullptr, nullptr, nullptr);
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

	for (auto vert : vertices) {
		verts.push_back(vert.getVec3());
	}
	for (auto col : colors) {
		cols.push_back(col.getVec3());
	}

	GLuint programId = program->getHandle();
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

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);

	if (fillSurface) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
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

	glBindVertexArray(0);
}

void PolyObject::draw(glm::mat4x4 mvp, GLenum mode) {

	program->use();
	program->setUniform("mvp", mvp);

	glPointSize(5.0f);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(mode, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
}

void PolyObject::rotateX() {
	globalFunctions.rotateXPointVector(vertices);
	updateCurveBuffer();
}
void PolyObject::rotateY() {
	globalFunctions.rotateYPointVector(vertices);
	updateCurveBuffer();
}
void PolyObject::rotateZ() {
	globalFunctions.rotateZPointVector(vertices);
	updateCurveBuffer();
}
void PolyObject::updateCurveBuffer() {
	this->verts.clear();
	for (auto& ptvector : vertices) {
		verts.push_back(ptvector.getVec3());
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

void PolyObject::selectPoint(glm::vec3& cameraPos, glm::vec3& rayVector) {

	double distance = INFINITY;

	//For each würde in diesem Fall nicht funktionieren
	for (int i = 0; i < vertices.size(); i++) {

		double numerator = glm::length(glm::cross((vertices.at(i).getVec3() - cameraPos), rayVector));
		double denumerator = glm::length(rayVector);
		double d = numerator / denumerator;

		if (d < distance) {
			distance = d;
			selectedPointVector = &(vertices.at(i));
		}
	}

	if (distance < globalConstants.SELECTION_OFFSET) {
		selectedPointNormal = rayVector;
		cout << "Selected: " << selectedPointVector->xCoor << "   " << selectedPointVector->yCoor << "   " << selectedPointVector->zCoor << endl;
	}
	else {
		selectedPointVector = nullptr;
		selectedPointNormal = glm::vec3(0.0f, 0.0f, 0.0f);
		cout << "No point Selected" << endl;
	}

}

bool PolyObject::dragPoint(glm::vec3& cameraPos, glm::vec3& rayVector) {
	double denominator = glm::dot(selectedPointNormal, rayVector);

	if (glm::abs(denominator) > 0.00001) {
		glm::vec3 difference = selectedPointVector->getVec3() - cameraPos;
		double t = glm::dot(difference, selectedPointNormal) / denominator;

		if (t > 0.0001) {
			glm::vec3 newRay = glm::vec3(t * rayVector.x, t * rayVector.y, t * rayVector.z);
			glm::vec3 newPoint = cameraPos + newRay;
			selectedPointVector->setVec3(newPoint, 1);

			return true;
		}
	}
	return false;
}

void PolyObject::togglePoints() {
	this->showPoints = !this->showPoints;
}

void PolyObject::toggleFillSurface()
{
	this->fillSurface = !this->fillSurface;
}

void PolyObject::setPoints(bool show) {
	showPoints = show;
}