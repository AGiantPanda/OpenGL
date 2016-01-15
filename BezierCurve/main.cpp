#include <iostream>

#include "GL/glew.h"
#include "GL/glut.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "myShader.h"

int WIDTH = 500, HEIGHT = 500;
GLuint vbo;
GLuint vao;
myShader curve;

GLfloat quad[] = {
	-2.0, 0.0, 1.0, 1.0, 1.0,
	-1.0, 2.0, 1.0, 1.0, 1.0,
	0.0, -2.0, 1.0, 1.0, 1.0,
	1.0, 2.0, 1.0, 1.0, 1.0,
	2.0, 0.0, 1.0, 1.0, 1.0
};

void updateVbo();
void initVbo();
void init();
void display();
void reshape(int width, int height);
void motion(int x, int y);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
	glutInitWindowPosition(520, 10);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Bezier Curve");
	GLenum err = glewInit();
	if (err != GLEW_OK){
		std::cout << "fail" << std::endl;
		return -1;
	}

	//multisample, is this realized by the fixed pipeline? I need to 
	//rewrite it in a shader with fbo maybe.
	GLint bufs, samples;
	glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
	glGetIntegerv(GL_SAMPLES, &samples);
	glEnable(GL_MULTISAMPLE);

	//show some info
	const GLubyte *name = glGetString(GL_VENDOR);
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *gluversion = gluGetString(GLU_VERSION);
	std::cout << "++++++++++++++++++++++++++++" << std::endl
		<< "+ GL_VENDOR: " << name << std::endl
		<< "+ GL_RENDERER: " << renderer << std::endl
		<< "+ GL_VERSION: " << version << std::endl
		<< "+ GLU_VERSION: " << gluversion << std::endl
		<< "++++++++++++++++++++++++++++" << std::endl;

	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutMainLoop();

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	return 0;
}

void updateVbo()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
}

void initVbo()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
}

void init()
{
	std::cout << curve.compileShader("./../curve.vs", GLSLShader::VERTEX);
	std::cout << curve.compileShader("./../curve.tcs", GLSLShader::TESS_CONTROL);
	std::cout << curve.compileShader("./../curve.tes", GLSLShader::TESS_EVALUATION);
	std::cout << curve.log();
	std::cout << curve.compileShader("./../curve.frag", GLSLShader::FRAGMENT);
	if (!curve.link()){
		printf("link failed.\n");
		std::cout << curve.log() << std::endl;
	}

	initVbo();
}

void display()
{
	glm::mat4 MVP = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f)
		* glm::lookAt(glm::vec3(0.0, 0.0, 5.0), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::mat4();

	curve.use();
	glBindVertexArray(vao);
	curve.setUniform("MVP", MVP);
	curve.setUniform("NumSegments", 64);
	curve.setUniform("NumStrips", 1);
	//curve.printActiveAttribs();
	//curve.printActiveUniforms();
	glViewport(0, 0, WIDTH, HEIGHT);
	glPatchParameteri(GL_PATCH_VERTICES, 5);//need to patch it first, here is 5 points/patch
	glDrawArrays(GL_PATCHES, 0, 5);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
	glutSwapBuffers();
}

void reshape(int width, int height)
{	
	WIDTH = width;
	HEIGHT = height;
	glutPostRedisplay();
}

void motion()
{

}