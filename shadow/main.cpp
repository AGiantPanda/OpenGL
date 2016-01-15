#include <Windows.h>
#include <map>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <SOIL.h>
#include <Camera.h>
#include "myShader.h"

#pragma comment(lib, "SOIL.lib")

int WIDTH = 600, HEIGHT = 600;
GLuint vertexID;
GLuint box_vao;
GLuint depthMap_fbo;
GLuint diffuseMap;
GLuint depthMap;
bool keys[1024];

GLuint vertexID2;
GLuint box_vao2;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
myShader light;
myShader depth;
myShader debugDepthQuad;
myShader mul;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

struct Light{
	bool state;
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	//for attenuation
	float constant;
	float linear;
	float quadratic;
	//for spotlight
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;	
};
Light fixedLight;

GLfloat cube[] = {
	//vertex    //normal    //texcoord
	-1, -1, -1,  0,  0, -1,  0, 0, //back
	-1,  1, -1,  0,  0, -1,  1, 0,
	 1,  1, -1,  0,  0, -1,  1, 1,
	 1, -1, -1,  0,  0, -1,  0, 1,

	-1, -1, -1,  0, -1,  0,  0, 0, //down
	 1, -1, -1,  0, -1,  0,  1, 0,
	 1, -1,  1,  0, -1,  0,  1, 1,
	-1, -1,  1,  0, -1,  0,  0, 1,

	-1, -1, -1, -1,  0,  0,  0, 0, //left
	-1, -1,  1, -1,  0,  0,  1, 0,
	-1,  1,  1, -1,  0,  0,  1, 1,
	-1,  1, -1, -1,  0,  0,  0, 1,

	 1,  1,  1,  1,  0,  0,  0, 0, //right
	 1, -1,  1,  1,  0,  0,  1, 0,
	 1, -1, -1,  1,  0,  0,  1, 1,
	 1,  1, -1,  1,  0,  0,  0, 1,
	  
	 1,  1,  1,  0,  1,  0,  0, 0, //top
	 1,  1, -1,  0,  1,  0,  1, 0,
	-1,  1, -1,  0,  1,  0,  1, 1,
	-1,  1,  1,  0,  1,  0,  0, 1,

	-1, -1,  1,  0,  0,  1,  0, 0, //front
	 1, -1,  1,  0,  0,  1,  1, 0,
	 1,  1,  1,  0,  0,  1,  1, 1,
	-1,  1,  1,  0,  0,  1,  0, 1,
};

void initVBO();
void initFBO();
void initLight();
void setFixedLight(Light light, myShader shader);
void initTexture();
void init();
void do_movement();
void display();
void reshape(int width, int height);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboarddown(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void idle();
void renderScene(myShader shader);
void renderQuad();

void initVBO2();
void init2();
void display2();
void reshape2(int width, int height);
void idle2();
void openwin2();

int window1, window2;
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
	glutInitWindowPosition(520, 10);
	glutInitWindowSize(WIDTH, HEIGHT);
	window1 = glutCreateWindow("shadow sample");
	GLenum err = glewInit();
	if(err != GLEW_OK){
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
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboarddown);
	glutKeyboardUpFunc(keyboardup);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	//openwin2();
	glutMainLoop();
	
	glDeleteVertexArrays(1, &box_vao);
	glDeleteBuffers(1, &vertexID);
	return 0;
}

void initVBO()
{
	//create vertex buffers
	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &box_vao);
	glBindVertexArray(box_vao);
	
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
}

void initFBO()
{
	glGenFramebuffers(1, &depthMap_fbo);

	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	//GL_DEPTH_COMPONENT - each element is a single depth value.
	//the GL converts it to floating point and clamps to the range [0, 1]
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMap_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE); 
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initLight()
{
	fixedLight.ambient = glm::vec3(0.4f, 0.4f, 0.4f);
	fixedLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	fixedLight.position = glm::vec3(5.0f, 5.0f, 5.0f);
	fixedLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	fixedLight.state = false;
}

void setFixedLight(Light light, myShader shader)
{
	shader.setUniform("fixedLight.diffuse", fixedLight.diffuse);
	shader.setUniform("fixedLight.ambient", fixedLight.ambient);
	shader.setUniform("fixedLight.position", fixedLight.position);
	shader.setUniform("fixedLight.specular", fixedLight.specular);
	shader.setUniform("fixedLight.state", fixedLight.state);
}

void initTexture()
{
	glGenTextures(1, &diffuseMap);
	int width, height;
	unsigned char *image;
	image = SOIL_load_image("./../wooden.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	if(!image)
		std::cout << "image read failed" << std::endl;
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void init()
{
	//init shader
	light.compileShader("./../light.vs", GLSLShader::VERTEX);
	light.compileShader("./../light.frag", GLSLShader::FRAGMENT);
	if(!light.link()){
		std::cout << "light shader link failed" << std::endl;
	}
	std::cout  << light.log() << std::endl;

	depth.compileShader("./../depth.vs", GLSLShader::VERTEX);
	depth.compileShader("./../depth.frag", GLSLShader::FRAGMENT);
	if (!depth.link()){
		std::cout << "depth shader not linked" << std::endl;
	}
	std::cout << depth.log() << std::endl;

	debugDepthQuad.compileShader("./../debugDepth.vs", GLSLShader::VERTEX);
	debugDepthQuad.compileShader("./../debugDepth.frag", GLSLShader::FRAGMENT);
	if (!debugDepthQuad.link()){
		std::cout << "debug shader not linked" << std::endl;
	}
	std::cout << debugDepthQuad.log() << std::endl;
	initVBO();
	initFBO();
	initLight();
	initTexture();
}
void do_movement()
{
	if (keys['w'])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys['s'])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys['a'])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys['d'])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void display()
{
	GLfloat currentFrame = GetTickCount();
	deltaTime = 0.1f;
	lastFrame = currentFrame;
	do_movement();
	
	
	glEnable(GL_DEPTH_TEST);

	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	lightProjection = glm::perspective(45.0f, (GLfloat)1024 / (GLfloat)1024, 0.1f, 100.0f);
	lightView = glm::lookAt(fixedLight.position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;
	depth.use();
	glBindVertexArray(box_vao);
	depth.setUniform("lightSpaceMatrix", lightSpaceMatrix);
	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMap_fbo);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	//use front face culling to remove peter panning
	//but here comes another question that cubes on the floor have
	//a non shadowed area very close to the cube,
	//this happens because of the bottom of the cube is too close to the floor
	glCullFace(GL_FRONT);
	renderScene(depth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//depth.printActiveAttribs();
	//depth.printActiveUniforms();

	glViewport(0, 0, WIDTH, HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	
	light.use();
	glBindVertexArray(box_vao);

	light.setUniform("material.shininess", 32.0f);
	setFixedLight(fixedLight, light);
	
	glm::mat4 view;
	view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	
	light.setUniform("view", view);
	light.setUniform("projection", projection);
	light.setUniform("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
	light.setUniform("lightSpaceMatrix", lightSpaceMatrix);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	light.setUniform("material.diffuse", 0);
	//glUniform1i(glGetUniformLocation(light.getHandle(), "material.diffuse"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	light.setUniform("depthMap", 1);

	//draw the wall & cube
	renderScene(light);
	//light.printActiveUniforms();
	//light.printActiveAttribs();

	//draw debug image
	//it has a image but barelly seen
	if (keys['0']){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		debugDepthQuad.use();
		debugDepthQuad.setUniform("depthMap", 1);
		renderQuad();
		//debugDepthQuad.printActiveAttribs();
		//debugDepthQuad.printActiveUniforms();
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
	glutSwapBuffers();
}

void reshape(int width, int height)
{
	WIDTH = width;
	HEIGHT = height;
}

bool firstMouse = true;
bool stateSwitch = false;
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x < 100 && y < 100){
		//HANDLE h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)openwin2, NULL, 1, 0);
		//ResumeThread(h);
		//Sleep(10000);
		//CloseHandle(h);
		openwin2();
	}
	if (button == GLUT_LEFT_BUTTON&&state == GLUT_DOWN){
		stateSwitch = false;
	}
	if (button == GLUT_LEFT_BUTTON&&state == GLUT_UP){
		stateSwitch = true;
	}
	if (stateSwitch) {
		ShowCursor(TRUE);
	}
	else{
		ShowCursor(FALSE);
		firstMouse = true;
	}
}
void motion(int x, int y)
{
	if (stateSwitch)
	{
		return;
	}
	if (firstMouse) {
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	GLfloat xoffset = x - lastX;
	GLfloat yoffset = lastY - y;

	lastX = x;
	lastY = y;
	camera.ProcessMouseMovement(xoffset, yoffset);
	glutPostRedisplay();
}

void keyboarddown(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);
	if (key >= 48 && key <= 57)
		keys[key] = !keys[key];
	else
		keys[key] = true;
}

void keyboardup(unsigned char key, int x, int y)
{
	if (!(key>=48 && key<=57))
		keys[key] = false;
}

void idle()
{
	//glutPostRedisplay();
}

GLuint quadVAO = 0;
GLuint quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void renderScene(myShader shader)
{
	glBindVertexArray(box_vao);

	glm::mat4 model;
	shader.setUniform("model", model);
	glDrawArrays(GL_QUADS, 0, 24);

	model = glm::translate(model, glm::vec3(-2.5f, -3.0f, 1.0f));
	shader.setUniform("model", model);
	glDrawArrays(GL_QUADS, 0, 24);

	model = glm::mat4();
	glm::mat4 zoom;
	zoom = glm::scale(zoom, glm::vec3(4.0));
	model = zoom*model;
	shader.setUniform("model", model);
	glDrawArrays(GL_QUADS, 0, 12);
}

void initVBO2()
{
	//create vertex buffers
	glGenBuffers(1, &vertexID2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	glGenVertexArrays(1, &box_vao2);
	glBindVertexArray(box_vao2);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
}

void init2()
{
	//init shader
	glutSetWindow(window2);
	mul.compileShader("./../multest.vs", GLSLShader::VERTEX);
	mul.compileShader("./../multest.frag", GLSLShader::FRAGMENT);
	if (!mul.link()){
		std::cout << "mul shader link failed" << std::endl;
	}
	std::cout << mul.log() << std::endl;

	initVBO2();
}

void display2()
{
	//glutSetWindow(window2);
	myShader test;
	test.compileShader("./../multest.vs", GLSLShader::VERTEX);
	test.compileShader("./../multest.frag", GLSLShader::FRAGMENT);
	if (!test.link()){
		std::cout << "test shader link failed" << std::endl;
	}
	std::cout << test.log() << std::endl;
	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(1.0, 1.0, 0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//glEnable(GL_DEPTH_TEST);

	test.use();
	
	glm::mat4 view;
	view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	glm::mat4 ortho = glm::ortho(-300.0f, 300.0f, -200.0f, 150.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(ortho));
	glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixf(glm::value_ptr(view));

	test.setUniform("view", view);
	test.setUniform("projection", projection);
	glBindVertexArray(box_vao2);
	glDrawArrays(GL_QUADS, 0, 24);

	glBindVertexArray(0);
	glUseProgram(0);
	glBegin(GL_POLYGON);
	glColor3f(1.0, 0.0, 0.0);
	glVertex2f(0.0, 0.0);
	glVertex2f(0.0, 150);
	glVertex2f(300, 0.0);
	glEnd();
	glutSwapBuffers();
}

void reshape2(int width, int height)
{
	WIDTH = width;
	HEIGHT = height;
}

void idle2()
{
	//glutPostRedisplay();
}

void openwin2(){
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
	GLenum err = glewInit();
	if (err != GLEW_OK){
		std::cout << "fail" << std::endl;
		return;
	}
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	glutInitWindowPosition(520, 10);
	glutInitWindowSize(WIDTH, HEIGHT);
	window2 = glutCreateWindow("shadow sample");
	init2();
	glutDisplayFunc(display2);
	glutReshapeFunc(reshape2);
	glutIdleFunc(idle2);
	//glutMainLoop();
}