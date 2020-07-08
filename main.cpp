#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>

#include <GL/glew.h>
//#include <GL/gl.h> // OpenGL header not necessary, included by GLEW
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "include/GLSLProgram.h"
#include "include/GLTools.h"

#include "header/ViewPanel.h"
#include <math.h>
#include "main.h"

#include "header/Gui.h"
#include <thread>


// Standard window width
const int WINDOW_WIDTH = 640;
int windowWidth = WINDOW_WIDTH;
// Standard window height
const int WINDOW_HEIGHT = 480;
int windowHeight = WINDOW_WIDTH;
// GLUT window id/handle
int glutID = 0;

cg::GLSLProgram program;

glm::mat4x4 view;
glm::mat4x4 projection;

ViewPanel* viewPanel = new ViewPanel(&program);
Gui* gui;

float zNear = 0.001f;
float zFar = 120.0f;
float eyeX = 0.0f;
float eyeY = 0.0f;
float eyeZ = 30.0f; // for view matrix (zoom)
float lookAtAngle = 45.0f;

bool needInit = false;
bool middleButtonPressed = false;

int xMoved = 0;
int yMoved = 0;

glm::vec3 eye(eyeX, eyeY, eyeZ);
glm::vec3 center(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);


/*
 Initialization. Should return true if everything is ok and false if something went wrong.
 */
bool init()
{
    // OpenGL: Set "background" color and enable depth testing.
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    // Construct view matrix.

    view = glm::lookAt(eye, center, up);

    // Create a shader program and set light direction.
    if (!program.compileShaderFromFile("shader/simple.vert", cg::GLSLShader::VERTEX)) {
        std::cerr << program.log();
        return false;
    }

    if (!program.compileShaderFromFile("shader/simple.frag", cg::GLSLShader::FRAGMENT)) {
        std::cerr << program.log();
        return false;
    }

    if (!program.link()) {
        std::cerr << program.log();
        return false;
    }

    // Create all objects.
    viewPanel->init();
    viewPanel->setView(view);

    return true;
}

/*
 Rendering.
 */
void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    viewPanel->draw();
}

void glutDisplay() {
    if (needInit) {
        init();
        needInit = false;
    }
    render();
    glutSwapBuffers();
}

/*
 Resize callback.
 */
void glutResize(int width, int height) {
	windowHeight = height;
	windowWidth = width;

    // Division by zero is bad...
    height = height < 1 ? 1 : height;
    glViewport(0, 0, width, height);

    // Construct projection matrix.
    projection = glm::perspective(lookAtAngle, (float)width / height, zNear, zFar);
    //projection = glm::ortho(-5.0F * width / height, 5.0F * width / height, -5.0F, 5.0F, zNear, zFar);

    viewPanel->setProjection(projection);
}

/*
 Callback for char input.
 */
void glutKeyboard(unsigned char keycode, int x, int y)
{
    switch (keycode) {
    case 27: // ESC
        glutDestroyWindow(glutID);
        return;
    case 'x':
		viewPanel->polyObjRotX();
        init();
        break;
    case 'y':
		viewPanel->polyObjRotY();
        init();
        break;
    case 'z':
		viewPanel->polyObjRotZ();
        init();
        break;
    case 'i':
        eyeZ--;
        eye.z = eyeZ;
        init();
        break;
    case 'o':
        eyeZ++;
        eye.z = eyeZ;
        init();
        break;
    }
    glutPostRedisplay();
}

/*
 Callback for mouse input.
 */
void glutMouse(int button, int state, int mousex, int mousey) {
    //Tutorial http://antongerdelan.net/opengl/raycasting.html
    float x = (2.0f * mousex) / windowWidth - 1.0f;
    float y = 1.0f - (2.0f * mousey) / windowHeight;
    float z = 1.0f;

    glm::vec3 ray_nds = glm::vec3(x, y, z);
    glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
    glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
    glm::vec3 ray_wor = (glm::inverse(view) * ray_eye);
    ray_wor = glm::normalize(ray_wor);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        viewPanel->selectPoint(eye, ray_wor);
	}

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        viewPanel->dragPoint(eye, ray_wor);
        viewPanel->drawStructure(0);
        viewPanel->drawStructure(1);
        init();
    }
    if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
        xMoved = mousex;
        yMoved = mousey;
        middleButtonPressed = true;
    }
    if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP){
        middleButtonPressed = false;
        xMoved = 0;
        yMoved = 0;
    }
  
	glutPostRedisplay();
}
void glutMotion(int x, int y) {
    if (middleButtonPressed) {
            eyeX += (xMoved - x)/1.5f;
            eye.x = eyeX;
            xMoved = x;

            eyeY += (yMoved - y)/1.5f;
            eye.y = eyeY;
            yMoved = y;
                  
            init();
    }
}
void mouseWheel (int button, int dir, int x, int y) {
    if (dir > 0) {
        eye = glm::rotateY(eye, 0.1f);
    } else {
        eye = glm::rotateY(eye, -0.1f);
    }
    init();
}


int main(int argc, char** argv) {


    // GLUT: Initialize freeglut library (window toolkit).
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(40, 40);
    glutInit(&argc, argv);



    // GLUT: Create a window and opengl context (version 4.3 core profile).
    glutInitContextVersion(4, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutCreateWindow("CADG");
    glutID = glutGetWindow();



    // GLEW: Load opengl extensions
    //glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return -1;
    }
#if _DEBUG
    if (glDebugMessageCallback) {
        std::cout << "Register OpenGL debug callback " << std::endl;
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(cg::glErrorVerboseCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE,
            GL_DONT_CARE,
            GL_DONT_CARE,
            0,
            nullptr,
            true); // get all debug messages
    }
    else {
        std::cout << "glDebugMessageCallback not available" << std::endl;
    }
#endif

    // GLUT: Set callbacks for events.
    glutReshapeFunc(glutResize);
    glutDisplayFunc(glutDisplay);
    glutIdleFunc(glutDisplay); // redisplay when idle
    glutKeyboardFunc(glutKeyboard);
	glutMouseFunc(glutMouse);
    glutMotionFunc(glutMotion);
    glutMouseWheelFunc(mouseWheel);

    // init vertex-array-objects.
    bool result = init();
    if (!result) {
        return -2;
    }

    // GLUT: Loop until the user closes the window
    // rendering & event handling
    gui = new Gui(viewPanel, glutPostRedisplay, &needInit);
    viewPanel->setGui(gui);
    std::thread th(&Gui::run, gui);
    glutMainLoop();
    th.join();

    // Cleanup in destructors:
    // Objects will be released in ~Object
    // Shader program will be released in ~GLSLProgram

    delete gui;
    delete viewPanel;

    return 0;
}
