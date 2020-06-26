#include "Bezier_Surface.h"
#include "header/ObjFileParser.h"
#include "header/DeCasteljau.h"
#include "header/Bernstein.h"

int lock = 0;

Bezier_Surface::Bezier_Surface(char* filename, cg::GLSLProgram* prog)
{
	ObjFileParser parser;
	obj = new PolyObject(prog);
	parser.parseObjectFile(filename, obj, &deg_m, &deg_n);
	//curves.push_back(new Bernstein(obj, prog));

	//for (auto obj : objs) {
	//	obj->setProgram(prog);
	//	Bernstein* curve = new Bernstein(obj, prog);
	//	//curve->calcCurve();
	//	curves.push_back(curve);
	//}

	for (int i = 0; i <= deg_n; i++) {
		PolyObject* c_obj = new PolyObject(prog);
		c_obj->setColor(PointVector(0.0f, 1.0f, 1.0f, 0.0f));
		for (int j = 0; j < deg_m; j++) {
			c_obj->pushIndex(j);
			c_obj->pushIndex(j + 1);
			c_obj->pushColor();
			c_obj->pushColor();
		}
		int k = i * deg_m * deg_n;
		while (k < (i + 1) * deg_m * deg_n) {
			if (k == i * deg_m * deg_n || k % 2 != 0) {
				c_obj->pushVertice(obj->getVertices().at(obj->getIndices().at(k)));
			}
			k++;
		}
		//c_obj->pushVertice(objs[objs.size() - 1]->getVertices().at(i));

		Bernstein* curve = new Bernstein(c_obj, prog);
		u_curves.push_back(curve);
	}
	calculateVCurves();

	rationalSurface(1, 1, 3);
}

Bezier_Surface::~Bezier_Surface()
{
}

vector<PolyObject*> Bezier_Surface::getPolyObjects()
{
	return objs;
}

vector<CurveBezier*> Bezier_Surface::getCurves()
{
	return u_curves;
}

void Bezier_Surface::pushObject(PolyObject* obj)
{
	objs.push_back(obj);
}

void Bezier_Surface::init()
{
	//obj->init();
	for (CurveBezier* curve : u_curves) {
		curve->init();
		curve->getControlStructure()->init();
		curve->getControlStructure()->setPoints(true);
		curve->getDerativeStructure()->init();
		//curve->getDeCasteljauStructure()->init();
	}
	for (CurveBezier* curve : v_curves) {
		curve->init();
		curve->getControlStructure()->init();
		curve->getControlStructure()->setPoints(true);
		curve->getDerativeStructure()->init();
		//curve->getDeCasteljauStructure()->init();
	}
}

void Bezier_Surface::draw(bool bezier_toggle, glm::mat4x4 projection, glm::mat4x4 view, glm::mat4x4 model) {
	while(lock == 1){ /* wait */ }
	lock = 1;
	for (CurveBezier* b : u_curves) {
		//Doppelte Verschachtelung nötig, da sonst nicht alle fälle abgedeckt
		if (bezier_toggle) {
			if (dynamic_cast<Bernstein*>(b)) {
				b->draw(projection * view * model);
				b->getControlStructure()->draw(projection * view * model);
				//b->getDeCasteljauStructure()->draw(projection * view * model);

				if (b->isDerivative() == 1) {
					b->getDerativeStructure()->draw(projection * view * model);
				}
			}
		}
		else if (dynamic_cast<DeCasteljau*>(b)) {
			b->draw(projection * view * model);
			b->getControlStructure()->draw(projection * view * model);
			//b->getDeCasteljauStructure()->draw(projection * view * model);

			if (b->isDerivative() == 1) {
				b->getDerativeStructure()->draw(projection * view * model);
			}
		}
	}

	for (CurveBezier* b : v_curves) {
		//Doppelte Verschachtelung nötig, da sonst nicht alle fälle abgedeckt
		if (bezier_toggle) {
			if (dynamic_cast<Bernstein*>(b)) {
				b->draw(projection * view * model);
				b->getControlStructure()->draw(projection * view * model);
				//b->getDeCasteljauStructure()->draw(projection * view * model);

				if (b->isDerivative() == 1) {
					b->getDerativeStructure()->draw(projection * view * model);
				}
			}
		}
		else if (dynamic_cast<DeCasteljau*>(b)) {
			b->draw(projection * view * model);
			b->getControlStructure()->draw(projection * view * model);
			//b->getDeCasteljauStructure()->draw(projection * view * model);

			if (b->isDerivative() == 1) {
				b->getDerativeStructure()->draw(projection * view * model);
			}
		}
	}
	lock = 0;
	//obj->draw(projection * view * model);
}

void Bezier_Surface::rationalSurface(int w_i, int w_j, float weight)
{
	CurveBezier* b = u_curves[w_i];
	if(dynamic_cast<Bernstein*>(b))
		b->calcRationalCurve(w_j, weight);
}

void Bezier_Surface::degree_increase_u()
{
	for (CurveBezier* curve : u_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			curve->degree_increase();
		}
	}

	v_curves.clear();
	deg_m++;
	calculateVCurves();
	//for (CurveBezier* curve : v_curves) {
	//	for (int i = 0; i < deg_n; i++) {
	//		if (dynamic_cast<Bernstein*>(curve)) {
	//			curve->degree_increase();
	//		}
	//	}
	//}
	//u_curves.clear();
	//deg_n++;
	//calculateUCurves();
}
void Bezier_Surface::degree_increase_v()
{
	for (CurveBezier* curve : v_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			curve->degree_increase();
		}
	}

	//v_curves.clear();
	//deg_m++;
	//calculateVCurves();
	//for (CurveBezier* curve : v_curves) {
	//	for (int i = 0; i < deg_n; i++) {
	//		if (dynamic_cast<Bernstein*>(curve)) {
	//			curve->degree_increase();
	//		}
	//	}
	//}
	u_curves.clear();
	deg_n++;
	calculateUCurves();
}

void Bezier_Surface::rotateX()
{
	for (CurveBezier* curve : u_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			curve->rotateX();
			curve->getControlStructure()->rotateX();
			//curve->getDeCasteljauStructure()->rotateX();
			curve->getDerativeStructure()->rotateX();
		}
	}
	for (CurveBezier* curve : v_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			curve->rotateX();
			curve->getControlStructure()->rotateX();
			//curve->getDeCasteljauStructure()->rotateX();
			curve->getDerativeStructure()->rotateX();
		}
	}
}

void Bezier_Surface::rotateY()
{
	for (CurveBezier* curve : u_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			curve->rotateZ();
			curve->getControlStructure()->rotateY();
			//curve->getDeCasteljauStructure()->rotateY();
			curve->getDerativeStructure()->rotateY();
		}
	}
	for (CurveBezier* curve : v_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			curve->rotateZ();
			curve->getControlStructure()->rotateY();
			//curve->getDeCasteljauStructure()->rotateY();
			curve->getDerativeStructure()->rotateY();
		}
	}
}

void Bezier_Surface::rotateZ()
{
	for (CurveBezier* curve : u_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			curve->rotateZ();
			curve->getControlStructure()->rotateZ();
			//curve->getDeCasteljauStructure()->rotateZ();
			curve->getDerativeStructure()->rotateZ();
		}
	}
	for (CurveBezier* curve : v_curves) {
		if (dynamic_cast<Bernstein*>(curve)) {
			curve->rotateZ();
			curve->getControlStructure()->rotateZ();
			//curve->getDeCasteljauStructure()->rotateZ();
			curve->getDerativeStructure()->rotateZ();
		}
	}
}

void Bezier_Surface::calculateVCurves()
{
	while (lock == 1) { /* wait */ }
	lock = 1;

	for (int i = 0; i <= deg_m; i++) {
		PolyObject* new_obj = new PolyObject(obj->getProgram());
		new_obj->setColor(PointVector(0.0f, 1.0f, 1.0f, 0.0f));
		for (int j = 0; j < u_curves.size() - 1; j++) {
			new_obj->pushVertice(u_curves[j]->getControlVertices().at(i));
			new_obj->pushIndex(j);
			new_obj->pushIndex(j + 1);
			new_obj->pushColor();
			new_obj->pushColor();
		}
		new_obj->pushVertice(u_curves[u_curves.size() - 1]->getControlVertices().at(i));
		v_curves.push_back(new Bernstein(new_obj, obj->getProgram()));
	}
	lock = 0;
}

void Bezier_Surface::calculateUCurves()
{
	while (lock == 1) { /* wait */ }
	lock = 1;

	for (int i = 0; i <= deg_n; i++) {
		PolyObject* new_obj = new PolyObject(obj->getProgram());
		new_obj->setColor(PointVector(0.0f, 1.0f, 1.0f, 0.0f));
		for (int j = 0; j < v_curves.size() - 1; j++) {
			new_obj->pushVertice(v_curves[j]->getControlVertices().at(i));
			new_obj->pushIndex(j);
			new_obj->pushIndex(j + 1);
			new_obj->pushColor();
			new_obj->pushColor();
		}
		new_obj->pushVertice(v_curves[v_curves.size() - 1]->getControlVertices().at(i));
		u_curves.push_back(new Bernstein(new_obj, obj->getProgram()));
	}
	lock = 0;
}

void Bezier_Surface::calculateUIndices()
{
	while (lock == 1) { /* wait */ }
	lock = 1;

	for (int i = 0; i <= deg_m; i++) {
		PolyObject* new_obj = new PolyObject(obj->getProgram());
		new_obj->setColor(PointVector(0.0f, 1.0f, 1.0f, 0.0f));
		for (int j = 0; j < v_curves.size() - 1; j++) {
			new_obj->pushIndex(j);
			new_obj->pushIndex(j + 1);
			new_obj->pushColor();
			new_obj->pushColor();
		}
		new_obj->pushVertice(v_curves[v_curves.size() - 1]->getControlVertices().at(i));
		u_curves.push_back(new Bernstein(new_obj, obj->getProgram()));
	}
	lock = 0;
}
void Bezier_Surface::calculateVIndices()
{
	while (lock == 1) { /* wait */ }
	lock = 1;

	for (int i = 0; i < u_curves.size(); i++) {
		PolyObject* new_obj = new PolyObject(obj->getProgram());
		new_obj->setColor(PointVector(0.0f, 1.0f, 1.0f, 0.0f));
		for (int j = 0; j < v_curves.size() - 1; j++) {
			new_obj->pushVertice(v_curves[j]->getControlVertices().at(i));
			new_obj->pushIndex(j);
			new_obj->pushIndex(j + 1);
			new_obj->pushColor();
			new_obj->pushColor();
		}
		new_obj->pushVertice(u_curves[u_curves.size() - 1]->getControlVertices().at(i));
		v_curves.push_back(new Bernstein(new_obj, obj->getProgram()));
	}
	lock = 0;
}


/*void Bezier_Surface::initObject(cg::GLSLProgram& program, const glm::vec3& color, float scale_factor) {
	initShader(1.0f, glm::vec4(0.0f, 0.0f, 4.0f, 0.0f), color, glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(1.0f, 1.0f, 1.0f), 10.0f);
	const std::vector<glm::vec3> vertices = edgelist.getVertices();
	const std::vector<glm::vec3> normals = edgelist.getNormals();

	std::vector < std::vector<glm::vec2>> faces = edgelist.getFaces();
	std::vector<glm::vec3> colors;
	std::vector<GLushort>  indices;
	std::vector<GLushort>  normal_indices;

	for (auto face : faces)
	{
		for (auto vert : face) {

			indices.push_back(vert.x - 1);
			normal_indices.push_back(vert.y);
		}
	}

	for (int i = 0; i < vertices.size(); i++) {
		colors.push_back(color);
	}

	sumObjInd = indices.size();

	GLuint programId = programPhongShaded.getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &object.vao);
	glBindVertexArray(object.vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &object.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, object.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &object.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, object.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Normal buffer.
	glGenBuffers(1, &object.normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, object.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

	pos = glGetAttribLocation(programId, "normal");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &object.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);

	// Modify model matrix.
	object.model = glm::mat4(1.0f);
	initNormals(program);
	initApproxNormals(program);
	initBoundingBox(program, vertices, scale_factor);
}

void Bezier_Surface::renderObject(cg::GLSLProgram& program, glm::mat4x4& view, glm::mat4x4& projection) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Create mvp.
	glm::mat4 mv = view * object.model;
	// Create mvp.
	glm::mat4 mvp = projection * mv;

	// Create normal matrix (nm) from model matrix.
	glm::mat3 nm = glm::inverseTranspose(glm::mat3(object.model));

	// Bind the shader program and set uniform(s).
	programPhongShaded.use();
	programPhongShaded.setUniform("modelviewMatrix", mv);
	programPhongShaded.setUniform("projectionMatrix", projection);
	programPhongShaded.setUniform("normalMatrix", nm);

	// Bind vertex array object so we can render the 1 triangle.
	glBindVertexArray(object.vao);
	glDrawElements(GL_TRIANGLES, sumObjInd, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}*/