#include <iostream>

//GL includes
#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

//local class
//#include "Ray.h"
//#include "mySphere.h"
//#include "PerspectiveCamera.h"

//others
#include"myShader.h"

using namespace std;

GLuint WIDTH = 800, HEIGHT = 800;
GLfloat quadVert[] = {
	-1.0f,  1.0f,
	-1.0f, -1.0f,
	 1.0f, -1.0f,

	-1.0f,  1.0f,
	 1.0f, -1.0f,
	 1.0f,  1.0f
};
GLuint quadVAO, quadVBO;
myShader screenShader;
//mySphere sphere(glm::vec3(), 5.0f);

void InitABO();
void Render();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


int main(int argc, char** argv)
{
	cout << "Starting GLFW context" << endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//create window object
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "RayTracing", nullptr, nullptr);
	if (!window)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//callback init
	glfwSetKeyCallback(window, key_callback);

	//disable the cursor
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//set this to true so GLEW know to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cout << "Failed to initialize GLEW" << endl;
		return -1;
	}
	//show info
	const GLubyte *name = glGetString(GL_VENDOR);
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	cout << "++++++++++++++++++++++++++++" << endl
		<< "+ GL_VENDOR   : " << name << endl
		<< "+ GL_RENDERER : " << renderer << endl
		<< "+ GL_VERSION  : " << version << endl
		<< "++++++++++++++++++++++++++++" << endl;
	
	//buffer init
	InitABO();
	//shader init
	screenShader.compileShader("./../RayTracing/RayTracing.vs", GLSLShader::VERTEX);
	cout << screenShader.log() << endl;
	screenShader.compileShader("./../RayTracing/RayTracing.fg", GLSLShader::FRAGMENT);
	cout << screenShader.log() << endl;
	if (!screenShader.link())
	{
		cout << "Shader link failed" << endl << screenShader.log() << endl;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	//gl loop
	while (!glfwWindowShouldClose(window))
	{
		//events detect
		glfwPollEvents();

		//render
		Render();
		
		//swap buffers
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

void InitABO()
{
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVert), &quadVert, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
}

void Render()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	screenShader.use();

	glm::mat4 model = glm::mat4();
	glm::mat4 view = glm::lookAt(
		glm::vec3(3.0, 3.0, 20.0), 
		glm::vec3(0.0, 0.0, 0.0), 
		glm::vec3(0.0, 1.0, 0.0));

	screenShader.setUniform("model", model);
	screenShader.setUniform("view", view);
	screenShader.setUniform("camera.fov", 45.0f);
	screenShader.setUniform("camera.aspect", (float)WIDTH/(float)HEIGHT);
	screenShader.setUniform("sphere.center", glm::vec3(0.0, 0.0, 0.0));
	screenShader.setUniform("sphere.radius", 5.0f);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}
