#define GLM_ENABLE_EXPERIMENTAL

#include "Object.h"
#include "FreeImage.h"
#include "glew.h"
#include "freeglut.h"
#include "shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"
#include <iostream>
#include <string>
#include <math.h>
#include <stb_image.h>

using namespace std;

void shaderInit();
void bindbufferInit();
void textureInit();
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);
void DrawBasis();
void DrawUmbreon();
void LoadTexture(unsigned int&, const char*);

GLuint program;
GLuint VAO, VBO[3];
unsigned int basistexture, modeltexture;
int windowSize[2] = { 600, 600 };
float angle = 0.0f;

Object* model = new Object("UmbreonHighPoly.obj");
// model->normals.size() == 44856

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("hw2");

	glewInit();
	shaderInit();
	bindbufferInit();
	textureInit();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void shaderInit()
{
	//// TODO: ////
	//
	// Hint:
	// 1. createShader
	// 2. createProgram
	GLuint vertex = createShader("Shaders/vertexShader.vert", "vertex");
	GLuint fragment = createShader("Shaders/fragmentShader.frag", "fragment");
	program = createProgram(vertex, fragment);
}

void bindbufferInit()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(3, VBO); // 0, 1, 2 = positions, normals, texcoords

	// set up positions
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->positions.size(), model->positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0);
	glEnableVertexAttribArray(0); // mark idx 0 being used

	// set up normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->normals.size(), model->normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0);
	glEnableVertexAttribArray(1);

	// set up texcoords
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->texcoords.size(), model->texcoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (GLvoid*)0);
	glEnableVertexAttribArray(2);

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void textureInit()
{
	LoadTexture(basistexture, "basis.jpg");
	LoadTexture(modeltexture, "Umbreon.jpg");
}

glm::mat4 getV()
{
	// set camera position and configuration
	return glm::lookAt(glm::vec3(7.5, 5.0, 7.5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

glm::mat4 getP()
{
	// set perspective view
	float fov = 60.0f;
	float aspect = windowSize[0] / windowSize[1];
	float nearDistance = 1.0f;
	float farDistance = 1000.0f;
	return glm::perspective(glm::radians(fov), aspect, nearDistance, farDistance);
}

void display()
{
	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawUmbreon();

	DrawBasis();

	angle += 0.1;
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

void LoadTexture(unsigned int& texture, const char* tFileName)
{
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(tFileName, &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
}

bool T = 0;

void keyboard(unsigned char key, int x, int y) // bonus
{
	switch (key)
	{
	case ' ':
		T = !T;
	}
}

void idle()
{
	glutPostRedisplay();
}

void DrawBasis()
{
	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], 1.0, 1000.0);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(7.5, 5.0, 7.5,  // eye
		0.0, 0.0, 0.0,  // center
		0.0, 1.0, 0.0); // up

	glPushMatrix();
	glRotatef(angle, 0, 1, 0);

	//// parameters
	int edge = 20;
	double r = 4;
	double pi = 3.1415926;
	double height = 3;

	// Top
	glBegin(GL_POLYGON);
	for (int i = 0; i < edge; i++)
	{
		double radian = 18 * pi / 180.0;
		glNormal3d(0.0, 1.0, 0.0);								 //normal vector
		glVertex3d(r * cos(i * radian), 0, r * sin(i * radian)); //20 vertex
	}
	glEnd();

	// Bottom
	glBegin(GL_POLYGON);
	for (int i = 0; i < edge; i++)
	{
		double radian = 18 * pi / 180.0;
		glNormal3d(0.0, -1.0, 0.0);									   //normal vector
		glVertex3d(r * cos(i * radian), -height, r * sin(i * radian)); //20 vertex
	}
	glEnd();

	// 20 Sides
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, basistexture);
	for (double i = 0; i < edge; i += 1)
	{
		glBegin(GL_POLYGON);
		double radian = 18 * pi / 180.0;
		double Top_right[3] = { r * cos(i * radian), 0, r * sin(i * radian) };
		double Bottom_right[3] = { r * cos(i * radian), -height, r * sin(i * radian) };
		double Top_left[3] = { r * cos((i + 1) * radian), 0, r * sin((i + 1) * radian) };
		double Bottom_left[3] = { r * cos((i + 1) * radian), -height, r * sin((i + 1) * radian) };
		double v1[3], v2[3]; // vectors for calculating normal
		for (int i = 0; i < 3; i++)
		{
			v1[i] = Top_left[i] - Top_right[i];
			v2[i] = Bottom_right[i] - Top_right[i];
		}
		double normal[3];
		//v1 cross v2 = normal
		normal[0] = v1[1] * v2[2] - v2[1] * v1[2];
		normal[1] = -(v1[0] * v2[2] - v2[0] * v1[2]);
		normal[2] = v1[0] * v2[1] - v2[0] * v1[1];
		//normalize
		double length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
		for (int i = 0; i < 3; i++)
		{
			normal[i] /= length;
		}

		glNormal3d(normal[0], normal[1], normal[2]);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3d(Top_right[0], Top_right[1], Top_right[2]);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3d(Bottom_right[0], Bottom_right[1], Bottom_right[2]);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3d(Bottom_left[0], Bottom_left[1], Bottom_left[2]);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3d(Top_left[0], Top_left[1], Top_left[2]);

		glEnd();
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(0);
	glPopMatrix();
}

void DrawUmbreon()
{
	glUseProgram(program);
	glBindVertexArray(VAO);
	// pass model matrix
	glm::mat4 M(1.0f);
	M = glm::rotate(M, glm::radians(angle), glm::vec3(0, 1, 0));
	M = glm::translate(M, glm::vec3(0, 1.3, 0));
	GLuint ModelMatrixID = glGetUniformLocation(program, "M");
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &M[0][0]);

	// pass projection matrix
	glm::mat4 P = getP();
	GLuint ProjectionMatrixID = glGetUniformLocation(program, "P");
	glUniformMatrix4fv(ProjectionMatrixID, 1, GL_FALSE, &P[0][0]);

	// pass view matrix
	glm::mat4 V = getV();
	GLuint ViewMatrixID = glGetUniformLocation(program, "V");
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &V[0][0]);

	// pass texture
	glActiveTexture(GL_TEXTURE0);
	if (!T) // bonus
		glBindTexture(GL_TEXTURE_2D, modeltexture);
	else
		glBindTexture(GL_TEXTURE_2D, basistexture);
	GLint texLoc = glGetUniformLocation(program, "TEX");
	glUniform1i(texLoc, 0);

	// draw model
	glDrawArrays(GL_QUADS, 0, 4 * model->fNum);

	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glActiveTexture(0);
	glUseProgram(0);
}