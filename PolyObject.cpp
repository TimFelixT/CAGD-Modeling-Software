#include "header/PolyObject.h"
#include "header/ObjFileParser.h"
#include <algorithm>
//#include "header/GlobalFunctions.h"

PolyObject::PolyObject()
{
}

PolyObject::PolyObject(cg::GLSLProgram* prog) : program(prog)
{
}

PolyObject::PolyObject(char* filename, cg::GLSLProgram* prog) : program(prog)
{
	ObjFileParser parser;
	parser.parseObjectFile(filename, this);
}

PolyObject::~PolyObject()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &positionBuffer);
}

void PolyObject::init() {
	vector<glm::vec3> verts;
	vector<glm::vec3> cols;

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


}
void PolyObject::draw(glm::mat4x4 mvp) {

	program->use();
	program->setUniform("mvp", mvp);


	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_LINES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

//void PolyObject::rotateX() {
//	globalFunctions.rotateXVec3(vertices);
//}
//void PolyObject::rotateY() {
//	globalFunctions.rotateYVec3(vertices);
//}
//void PolyObject::rotateZ() {
//	globalFunctions.rotateZVec3(vertices);
//}

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

void PolyObject::pushFace(vector<PointVector> face)
{
	faces.push_back(face);
}

void PolyObject::pushIndex(GLushort index)
{
	indices.push_back(index);
}

void PolyObject::pushVertice(PointVector vertice)
{
	vertices.push_back(vertice);
	colors.push_back(PointVector(0.0f, 1.0f, 0.0f, 0.0f));
}

void PolyObject::pushNormal(PointVector normal)
{
	glm::vec3 norm = glm::normalize(normal.getVec3());

	normals.push_back(PointVector(norm.x, norm.y, norm.z, 0));
}

std::vector<PointVector> PolyObject::getVertices()
{
	return vertices;
}

std::vector<PointVector> PolyObject::getNormals()
{
	return normals;
}

std::vector<std::vector<PointVector>> PolyObject::getFaces()
{
	return faces;
}