/*
*phong lighting with arcball rotation
*Depth testing - 15.10.30
*FBO - 15.10.30
*/

#include <ctime>
#include <iostream>

#include <GL/glew.h>
#include <GL/glut.h>
#include "SOIL.h"
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Arcball.h"
#include "myShader.h"
#include "model.h"

bool stateSwitch = false;
GLuint vertexID;
GLuint vao;
GLuint posLocID;
GLuint normalLocID;
GLuint texLocID;
GLuint vShader, fShader, programHandle;
GLuint diffuseMap;
GLuint specularMap;
GLuint fbo;
myShader shader;
int imageWidth = 800, imageHeight = 800;
int windowWidth = imageWidth, windowHeight = imageHeight, windowX = 100, windowY = 100;
const GLuint WIDTH = 800, HEIGHT = 800;
Arcball arcball(WIDTH/2, glm::vec2(WIDTH/2, HEIGHT/2));
GLfloat lastX = 0.0f;
GLfloat lastY = 0.0f;

GLfloat firstTime = GetTickCount();
GLfloat secondTime = 0.0f;

GLfloat vertices[] = {
	//vertex
	0, -4, 0,
};





void initVBO()//vbo的生成与vao的绑定
{
	//create vertex buffers
	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	//创建顶点数组
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//顶点属性数组
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindVertexArray(0);
}

Model crytek("D:/VSProject/assimp/nanosuit/nanosuit.obj");
void init()
{
	shader.compileShader("./../model.vs", GLSLShader::VERTEX);
	std::cout << shader.log() << std::endl;
	shader.compileShader("./../model.frag", GLSLShader::FRAGMENT);
	std::cout << shader.log() << std::endl;
	if (!shader.link()){
		std::cout << "shader link failed\n" << std::endl;
		std::cout << shader.log() << std::endl;
		system("pause");
	}
	shader.printActiveAttribs();
	shader.printActiveUniforms();

	
}

void display()
{
	static int frame = 0;
	frame++;
	secondTime = GetTickCount();
	if (secondTime - firstTime >= 1000){
		std::cout << "frame: " << frame << std::endl;
		firstTime = secondTime;
		frame = 0;
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.use();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);
	//use vaoID & draw quads

	glm::mat4 view;
	view = glm::lookAt(glm::vec3(0.0, 0.0, 10.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 projection = glm::perspective(GLfloat(45.0), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	glm::mat4 rotate = arcball.GetArcballMatrix();
	glm::mat4 model;

	shader.setUniform("projection", projection);
	shader.setUniform("view", view);
	shader.setUniform("model", model);
	
	crytek.Draw(shader);

	glUseProgram(0);
	//glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height)
{
	windowWidth = width;
	windowHeight = height;
}

bool firstMouse = true;
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON&&state == GLUT_DOWN){
		lastX = (GLfloat)x;
		lastY = (GLfloat)y;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP){
	}
	if (button == GLUT_RIGHT_BUTTON&&state == GLUT_DOWN){
		lastX = (GLfloat)x;
		lastY = (GLfloat)y;
	}
	if (button == GLUT_RIGHT_BUTTON&&state == GLUT_UP){
	}
	glutPostRedisplay();
}

void motion(int x, int y)
{
	lastX = (GLfloat)x;
	lastY = (GLfloat)y;
	glutPostRedisplay();
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
	glutInitWindowPosition(windowX, windowY);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("wheel");
	GLenum err = glewInit();
	if (err != GLEW_OK){
		std::cout << "fail" << std::endl;
		return -1;
	}

	//多重采样 抗锯齿
	GLint bufs, samples;   //支持多重采样时，值不能为零
	glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
	glGetIntegerv(GL_SAMPLES, &samples);
	glEnable(GL_MULTISAMPLE);
	const GLubyte* name = glGetString(GL_VENDOR);
	const GLubyte* biaoshifu = glGetString(GL_RENDERER);
	const GLubyte* OpenGLVersion = glGetString(GL_VERSION);
	const GLubyte* gluVersion = gluGetString(GLU_VERSION);
	std::cout << "GL_VENDER: " << name << std::endl
		<< "GL_RENDERER: " << biaoshifu << std::endl
		<< "GL_VERSION: " << OpenGLVersion << std::endl
		<< "gluVertion: " << gluVersion << std::endl;
	init();
	glViewport(0, 0, WIDTH, HEIGHT);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	glutMainLoop();

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vertexID);
	glDeleteProgram(programHandle);
	glDeleteShader(vShader);
	glDeleteShader(fShader);
	return 0;
}