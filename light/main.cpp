/*
*phong lighting with arcball rotation
*Depth testing - 15.10.30
*FBO - 15.10.30
*/

#include <GL/glew.h>
#include <GL/glut.h>
#include <ctime>
#include "shader.h"
#include <iostream>
#include "SOIL.h"
#include "myShader.h"
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Arcball.h"

#pragma comment(lib, "SOIL.lib")

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
int imageWidth = 800, imageHeight = 800;
int windowWidth = imageWidth, windowHeight = imageHeight, windowX = 100, windowY = 100;
const GLuint WIDTH = 800, HEIGHT = 800;
myShader test;
Arcball arcball(WIDTH/2, glm::vec2(WIDTH/2, HEIGHT/2));
GLfloat lastX = 0.0f;
GLfloat lastY = 0.0f;

GLfloat firstTime = GetTickCount();
GLfloat secondTime = 0.0f;

GLfloat vertices[] = {
	//vertex    //normal    //texcoord
	-1, -1, -1,  0,  0, -1,  0, 0,
	-1,  1, -1,  0,  0, -1,  1, 0,
	 1,  1, -1,  0,  0, -1,  1, 1,
	 1, -1, -1,  0,  0, -1,  0, 1,

	-1, -1,  1,  0,  0,  1,  0, 0,
	 1, -1,  1,  0,  0,  1,  1, 0,
	 1,  1,  1,  0,  0,  1,  1, 1,
	-1,  1,  1,  0,  0,  1,  0, 1,

	-1, -1, -1, -1,  0,  0,  0, 0,
	-1, -1,  1, -1,  0,  0,  1, 0,
	-1,  1,  1, -1,  0,  0,  1, 1,
	-1,  1, -1, -1,  0,  0,  0, 1,

	 1,  1,  1,  1,  0,  0,  0, 0,
	 1, -1,  1,  1,  0,  0,  1, 0,
	 1, -1, -1,  1,  0,  0,  1, 1,
	 1,  1, -1,  1,  0,  0,  0, 1,
	  
	 1,  1,  1,  0,  1,  0,  0, 0,
	 1,  1, -1,  0,  1,  0,  1, 0,
	-1,  1, -1,  0,  1,  0,  1, 1,
	-1,  1,  1,  0,  1,  0,  0, 1,

	-1, -1, -1,  0, -1,  0,  0, 0,
	 1, -1, -1,  0, -1,  0,  1, 0,
	 1, -1,  1,  0, -1,  0,  1, 1,
	-1, -1,  1,  0, -1,  0,  0, 1,
};

void printInfoLog(GLuint shader)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 1) {
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(shader, infologLength,
			&charsWritten, infoLog);
		printf("%s \n", infoLog);
		printf("***********************************************************\n");
		free(infoLog);
	}
}

void initFBO()
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 800, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
}

void initVBO()//vbo的生成与vao的绑定
{
	//create vertex buffers
	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//创建顶点数组
	//将VBO对象放进VAO里。VAO全称是Vectex Array Object,它保存了一次绘制所需要的全部顶点信息,包括顶点坐标、法线、顶点索引、颜色等VBO的句柄,以及相应的顶点坐标、法线、顶点索引、颜色的数据格式(通过glVertexAttribPointer设置)和 shader-attribute-location 是否启用(通过glVertexAttribPointer设置)。
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//顶点属性数组
	glEnableVertexAttribArray(posLocID);//默认情况下，着色器(shader)里对应属性位置(location)的顶点访问都是关闭的，我们要通过设置glEnableVertexAttribArray来设置其允许访问
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glVertexAttribPointer(posLocID, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);

	//颜色属性数组
	glEnableVertexAttribArray(normalLocID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glVertexAttribPointer(normalLocID, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glEnableVertexAttribArray(texLocID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glVertexAttribPointer(texLocID, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
}

void initGLSL()
{
	//create & compile a vertex shader
	vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vertl, NULL);
	glCompileShader(vShader);
	printInfoLog(vShader);

	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fragl, NULL);
	glCompileShader(fShader);
	printInfoLog(fShader);

	//create a glslhandle & attach shaders
	programHandle = glCreateProgram();
	glAttachShader(programHandle, vShader);
	glAttachShader(programHandle, fShader);
	glLinkProgram(programHandle);

	GLint success;
	glGetProgramiv(programHandle,
		GL_LINK_STATUS,
		&success);
	if (!success) {
		printf("Shader could not be linked!\n ");
	}

	//bind attribute
	posLocID = glGetAttribLocation(programHandle, "vertexPosition");//一种绑定
	normalLocID = glGetAttribLocation(programHandle, "vertexNormal");
	texLocID = glGetAttribLocation(programHandle, "texCoords");
}

void init()
{
	//gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	//glClearColor(0.0, 0.0, 0.0, 0.0);
	test.compileShader("D:/VSProject/lighting/test.vs", GLSLShader::VERTEX);
	test.compileShader("D:/VSProject/lighting/test.frag", GLSLShader::FRAGMENT);
	test.link();
	std::cout << test.isLinked() << std::endl;
	test.bindAttribLocation(0, "vertexPosition");
	test.bindAttribLocation(1, "vertexNormal");
	test.bindAttribLocation(5, "texCoords");
	initGLSL();
	initVBO();
	glGenTextures(1, &diffuseMap);
	int width, height;
	unsigned char* image;
	image = SOIL_load_image("woodbox.png", &width, &height, 0, SOIL_LOAD_RGB);
	if (!image)
		std::cout << "image read failed" << std::endl;
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	glGenTextures(1, &specularMap);
	unsigned char* image2;
	image2 = SOIL_load_image("woodbox_spec.png", &width, &height, 0, SOIL_LOAD_RGB);
	if (!image2) std::cout << "spec read failed" << std::endl;
	glBindTexture(GL_TEXTURE_2D, specularMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
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
	//glUseProgram(programHandle);
	test.use();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//use vaoID & draw quads
	glBindVertexArray(vao);
	glm::mat4 view;
	view = glm::lookAt(glm::vec3(0.0, 0.0, 8.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 projection = glm::perspective(GLfloat(45.0), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	glm::mat4 rotate = arcball.GetArcballMatrix();
	glm::mat4 model;
	glm::mat4 invrotate = glm::transpose(glm::inverse(rotate));

	//glUniform3f(glGetUniformLocation(test.getHandle(), "light.diffuse"), 1.0f, 1.0f, 1.0f);
	//glUniform3f(glGetUniformLocation(test.getHandle(), "light.ambient"), 0.2f, 0.2f, 0.2f);
	//glUniform3f(glGetUniformLocation(test.getHandle(), "light.specular"), 1.0f, 1.0f, 1.0f);
	//glUniform3f(glGetUniformLocation(test.getHandle(), "light.position"), 0.5f,0.5f, 2.0f);
	test.setUniform("light.diffuse", 1.0f, 1.0f, 1.0f);
	test.setUniform("light.ambient", 0.2f, 0.2f, 0.2f);
	test.setUniform("light.specular", 1.0f, 1.0f, 1.0f);
	test.setUniform("light.position", 0.5f, 0.5f, 2.0f);

	glUniform3f(glGetUniformLocation(test.getHandle(), "material.specular"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(test.getHandle(), "material.shininess"), 32.0f);

	GLint modelLoc = glGetUniformLocation(test.getHandle(), "model");
	GLint viewLoc = glGetUniformLocation(test.getHandle(), "view");
	GLint projLoc = glGetUniformLocation(test.getHandle(), "projection");
	GLint viewPosLoc = glGetUniformLocation(test.getHandle(), "viewPos");
	GLint rotateLoc = glGetUniformLocation(test.getHandle(), "rotate");
	GLint invrotateLoc = glGetUniformLocation(test.getHandle(), "invrotate");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(rotateLoc, 1, GL_FALSE, glm::value_ptr(rotate));
	glUniformMatrix4fv(invrotateLoc, 1, GL_FALSE, glm::value_ptr(invrotate));

	glUniform3f(viewPosLoc, 0.0, 0.0, 8.0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glUniform1i(glGetUniformLocation(programHandle, "material.diffuse"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	glUniform1i(glGetUniformLocation(programHandle, "material.specular"), 1);
	glDrawArrays(GL_QUADS, 0, 24);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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
		arcball.mouse_down(LEFTBUTTON_DOWN);
		lastX = (GLfloat)x;
		lastY = (GLfloat)y;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		arcball.mouse_up(LEFTBUTTON_UP);
	}
	if (button == GLUT_RIGHT_BUTTON&&state == GLUT_DOWN){
		arcball.mouse_down(RIGHTBUTTON_DOWN);
		lastX = (GLfloat)x;
		lastY = (GLfloat)y;
	}
	if (button == GLUT_RIGHT_BUTTON&&state == GLUT_UP){
		arcball.mouse_up(RIGHTBUTTON_UP);
	}
	glutPostRedisplay();
}

void motion(int x, int y)
{
	arcball.mouse_motion(lastX, lastY, (GLfloat)x, ((GLfloat)y));
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