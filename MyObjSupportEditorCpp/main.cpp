#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>
#include <iostream>	

using namespace std;
#include "TestData.h"
#include <string>
#include "Model.h"
#include "ShaderProgramm.h"


#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"opengl32.lib")

int getSign(int s) {
	if (s >0) {
		return 1;
	}
	else{
		return -1;
	}
}

double angle = 0.f;
double moveFactor = 0.01f;
double rotateFactor = 1.f;


double x = 0, y = 0, z = 0;
double rX = 0, rY = 0, rZ = 0;
double angleYAxis = 0, angleXAxis = 0;

enum class Mode
{
	Move,
	Rotation
};

enum class NavigationMode //для переключения режимов работы при манипулировании мышкой
{
	Movee,
	Rotate,
	Default
};

Mode currentMode = Mode::Move;

NavigationMode currentNavigationMode = NavigationMode::Default;

std::vector<Model> testVector;

void key_callback(GLFWwindow * window,int key,int scancode,int action,int mods) {
	
}
double ox, oy, oz;

void FromClientToWorld(double xpos, double ypos, double* ox, double* oy, double* oz) {
	int vp[4];
	double mMatrix[16], pMatrix[16];
	float z;

	glGetIntegerv(GL_VIEWPORT, vp);
	ypos = vp[3] - ypos - 1;

	glGetDoublev(GL_MODELVIEW_MATRIX, mMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, pMatrix);
	glReadPixels(xpos, ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

	gluUnProject(xpos, ypos, z, mMatrix, pMatrix, vp, ox, oy, oz);
}
void MouseButtonCallback(GLFWwindow *window,int button,int action,int mods)
{	
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			currentNavigationMode = NavigationMode::Movee;
			//click by left mouse button
			//сперва должны определять, на что нажали, а уже потом что делать, в зависимости от того во что нажали

			double mousePosX, mousePosY;
			glfwGetCursorPos(window, &mousePosX, &mousePosY);
			FromClientToWorld(mousePosX,mousePosY, &ox, &oy, &oz);

			testVector.push_back(Model("s.stl",ox,oy,oz,true));
			 
		}
		else if (action == GLFW_RELEASE) {
			currentNavigationMode = NavigationMode::Default;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			currentNavigationMode = NavigationMode::Rotate;
		}
		else if (action == GLFW_RELEASE) {
			currentNavigationMode = NavigationMode::Default;
		}
	}
}


double lastX, lastY;
void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
	
	glfwGetCursorPos(window, &xpos, &ypos);
	if (currentNavigationMode == NavigationMode::Movee) {
		
		x += moveFactor * getSign(xpos-lastX);
		y -= moveFactor * getSign(ypos - lastY);
		lastX = xpos;
		lastY = ypos;
		
	}
	else if (currentNavigationMode == NavigationMode::Rotate) {

		angleYAxis += moveFactor * 100 * getSign(xpos - lastX);
		angleXAxis += moveFactor * 100 * getSign(ypos - lastY);
		
		lastX = xpos;
		lastY = ypos;
	}
	glfwGetCursorPos(window, &xpos, &ypos);
	FromClientToWorld(xpos, ypos, &ox, &oy, &oz);
	//std::cout << "(" << ox << "),(" << oy << "),(" << oz << ")\n";
}

void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	z += yoffset * 0.1f;
}

int main() {
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(900, 600, "sdsd", nullptr, nullptr);
	if (window == nullptr)
	{
		//cout << "Failed init window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cout << "Failed to init GLEW" << endl;
		return -1;
	}
	glViewport(0, 0, 900, 600);

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPositionCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);

	Model m("sus.stl",0,0,0,false);

	//секция со светом
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.1,0.1,-0.1,0.1,0.1,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);

	Model::supportNameId = 200;

	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		// Render
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		float direction[] = { 0,0,1,0 };
		glLightfv(GL_LIGHT0, GL_POSITION, direction);
		glTranslatef(x, y, z);
		glRotated(angleYAxis,0, 1, 0);
		glRotated(angleXAxis, 1, 0, 0);
		static float rad = 0;
		//block of rotate camera

		glInitNames();

		m.DrawWithoutEbo();
		for (Model model : testVector) {
			model.DrawWithoutEbo();
		}
		// Swap the screen buffersf
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

