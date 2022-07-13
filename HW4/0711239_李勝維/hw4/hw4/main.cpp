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
void DrawUmbreon(GLuint);
void LoadTexture(unsigned int&, const char*);

GLuint Normalprogram, explode_program;
GLuint VAO, VBO[3];
unsigned int modeltexture;
float windowSize[2] = { 600, 600 };
float angle = 0.0f;
glm::vec3 WorldLightPos = glm::vec3(2, 5, 5);
glm::vec3 WorldCamPos = glm::vec3(15, 5, 5);

Object* model = new Object("Umbreon.obj");

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("hw4");

	glewInit();
	shaderInit();
	bindbufferInit();
	textureInit();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void shaderInit() {
	GLuint vert = createShader("Shaders/normal.vert", "vertex");
	GLuint geom;
	GLuint frag = createShader("Shaders/normal.frag", "fragment");
	Normalprogram = createProgram(vert, 0,frag);

	vert = createShader("Shaders/Umbreon.vert", "vertex");
	geom = createShader("Shaders/Umbreon.geom", "geometry");
	frag = createShader("Shaders/Umbreon.frag", "fragment");
	explode_program = createProgram(vert, geom, frag);
}

void bindbufferInit() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(3, VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model->positions[0]) * model->positions.size(), &model->positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model->normals[0]) * model->normals.size(), &model->normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model->texcoords[0]) * model->texcoords.size(), &model->texcoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}




void textureInit() {
	LoadTexture(modeltexture, "Umbreon.jpg");
}

glm::mat4 getV()
{
	// set camera position and configuration
	return glm::lookAt(glm::vec3(WorldCamPos.x, WorldCamPos.y, WorldCamPos.z), glm::vec3(1, 1, 2), glm::vec3(0, 1, 0));
}

glm::mat4 getP()
{
	// set perspective view
	float fov = 45.0f;
	float aspect = windowSize[0] / windowSize[1];
	float nearDistance = 1.0f;
	float farDistance = 1000.0f;
	return glm::perspective(glm::radians(fov), aspect, nearDistance, farDistance);
}

float now_time = 1;
float speed = 0.03;
int stage = 0;
glm::vec3 pos(1.0, 1.0, -10.0);

glm::mat4 getM()
{
	glm::mat4 M(1.0f);
	if (stage == 0)
	{
		if ((pos.z <= 0 || sin(angle) >= 0.01))
		{
			pos.z += speed;
			angle += 0.04f;
		}
		else
		{
			stage++;
			angle = 0;
		}
		M = glm::translate(M, pos); // near , up , left
		M = glm::rotate(M, glm::radians(sin(angle) * 30), glm::vec3(1, 0, 0));
	}
	else if (stage == 1)
	{
		angle += 1;
		M = glm::translate(M, pos);
		M = glm::rotate(M, glm::radians(angle), glm::vec3(0, 1, 0));
		if (angle >= 360)
			stage++;
	}
	else if (stage == 2)
	{
		angle -= 1.5;
		M = glm::translate(M, pos);
		M = glm::rotate(M, glm::radians(angle), glm::vec3(0, 1, 0));
		if (angle <= 0)
		{
			stage++;
			angle = 0;
		}
	}
	else if (stage == 3)
	{
		now_time += speed;
		M = glm::translate(M, pos);
		if (now_time >= 19)
			stage++;
	}
	else if (stage == 4)
	{
		now_time -= speed;
		speed += 0.02 * speed;
		speed = min(0.25f, speed);
		M = glm::translate(M, pos);
		if (now_time <= 0)
			stage++;
	}
	else if (stage == 5)
	{
		pos.z += 0.05;
		angle += 0.04f;
		M = glm::translate(M, pos); // near , up , left
		M = glm::rotate(M, glm::radians(sin(angle) * 30), glm::vec3(1, 0, 0));
	}
	return M;
}

void display() {
	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (stage == 3 || stage == 4)
		DrawUmbreon(explode_program);
	else
		DrawUmbreon(Normalprogram);
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void LoadTexture(unsigned int& texture, const char* tFileName) {
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

void idle() {
	glutPostRedisplay();
}

void DrawUmbreon(GLuint program)
{
	glUseProgram(program);

	glm::mat4 M = getM();

	GLuint ModelMatrixID = glGetUniformLocation(program, "M");
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &M[0][0]);

	glm::mat4 V = getV();
	ModelMatrixID = glGetUniformLocation(program, "V");
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &V[0][0]);

	glm::mat4 P = getP();
	ModelMatrixID = glGetUniformLocation(program, "P");
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &P[0][0]);

	glUniform1f(glGetUniformLocation(program, "time"), now_time);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, modeltexture);
	glUniform1i(glGetUniformLocation(program, "texture"), 0);

	
	// parameters for Phong
	glm::vec3 Ka(1, 1, 1), Kd(1, 1, 1), Ks(1, 1, 1), La(0.5f, 0.5f, 0.5f), Ld(0.8f, 0.8f, 0.8f), Ls(0.5f, 0.5f, 0.5f);
	float gloss = 25.0f;
	glUniform3fv(glGetUniformLocation(program, "worldCam"), 1, &WorldCamPos[0]);
	glUniform3fv(glGetUniformLocation(program, "worldLight"), 1, &WorldLightPos[0]);

	glUniform3fv(glGetUniformLocation(program, "Ka"), 1, &Ka[0]);
	glUniform3fv(glGetUniformLocation(program, "Kd"), 1, &Kd[0]);
	glUniform3fv(glGetUniformLocation(program, "Ks"), 1, &Ks[0]);

	glUniform3fv(glGetUniformLocation(program, "La"), 1, &La[0]);
	glUniform3fv(glGetUniformLocation(program, "Ld"), 1, &Ld[0]);
	glUniform3fv(glGetUniformLocation(program, "Ls"), 1, &Ls[0]);

	glUniform1f(glGetUniformLocation(program, "gloss"), gloss);
	

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0,  3*model->fNum);
	glBindVertexArray(0);
	glActiveTexture(0);
	glUseProgram(0);
}


