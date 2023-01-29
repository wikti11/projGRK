﻿#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Spline.cpp"

#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

int WIDTH = 500, HEIGHT = 500;

namespace models
{
	Core::RenderContext columnMiddleContext;
	Core::RenderContext columnNEContext;
	Core::RenderContext columnNWContext;
	Core::RenderContext columnSEContext;
	Core::RenderContext columnSWContext;
	Core::RenderContext doorLeftContext;
	Core::RenderContext doorRightContext;
	Core::RenderContext torchEContext;
	Core::RenderContext torchNEContext;
	Core::RenderContext roomContext;
	Core::RenderContext spaceshipContext;
	Core::RenderContext torchNWContext;
	Core::RenderContext torchSEContext;
	Core::RenderContext torchSWContext;
	Core::RenderContext torchWContext;

	Core::RenderContext room;
	Core::RenderContext dragonSphere;
	Core::RenderContext	dragonCone;
	Core::RenderContext dragonCylinder;
}

GLuint depthMapFBO;
GLuint depthMap;

GLuint program;
GLuint programSun;
GLuint programTest;
GLuint programTex;

Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;

glm::vec3 sunPos = glm::vec3(-4.740971f, 2.149999f, 0.369280f);
glm::vec3 sunDir = glm::vec3(-0.93633f, 0.351106, 0.003226f);
glm::vec3 sunColor = glm::vec3(0.9f, 0.9f, 0.7f) * 5;

glm::vec3 cameraPos = glm::vec3(0.479490f, 1.250000f, -2.124680f);
glm::vec3 cameraDir = glm::vec3(-0.354510f, 0.000000f, 0.935054f);

glm::vec3 spaceshipPos = glm::vec3(0.065808f, 1.250000f, -2.189549f);
glm::vec3 spaceshipDir = glm::vec3(-0.490263f, 0.000000f, 0.871578f);
GLuint VAO, VBO;

float aspectRatio = 1.f;

float exposition = 1.f;

glm::vec3 pointlightPos = glm::vec3(0, 2, 0);
glm::vec3 pointlightColor = glm::vec3(0.9, 0.6, 0.6);

glm::vec3 spotlightPos = glm::vec3(0, 0, 0);
glm::vec3 spotlightConeDir = glm::vec3(0, 0, 0);
glm::vec3 spotlightColor = glm::vec3(0.4, 0.4, 0.9) * 3;
float spotlightPhi = 3.14 / 4;

float lastTime = -1.f;
float deltaTime = 0.f;

namespace dragon
{
	bool hasBeenShot = false;

	Spline path;
	int n = 1000;
	float headIndex = 50;
	float body1Index = 40;
	float body2Index = 35;
	float body3Index = 30;
	float tailIndex = 25;
	float step = 0.0f;

	float theta = 0.0f;
	std::vector<glm::vec3> normal;
	std::vector<glm::vec3> tangent;
	std::vector<glm::vec3> bitangent;
}

namespace arrow
{
	glm::vec3 position = glm::vec3(1.0f);
	glm::vec3 direction = glm::vec3(1.0f);
	glm::vec3 side = glm::vec3(1.0f);
	glm::vec3 up = glm::vec3(1.0f);
	glm::mat4 cameraRotrationMatrix = glm::mat4(1.0f);
	float time = 1.0f;
	float radius = 0.5f;
	float flySpeed = 1.0f;
	float fallSpeed = 1.0f;
	bool hasBeenShot = false;
}

void initDragon()
{
	dragon::path.points = {
	{  0.0f, 1.0f, 0.0f },
	{  0.5f, 1.0f, 0.0f },
	{  0.5f, 1.0f, 0.5f },
	{  0.5f, 1.0f, 1.0f },
	{  0.5f, 1.0f, 1.5f },
	{  0.5f, 1.0f, 2.0f },
	{  0.0f, 1.0f, 2.0f },
	{ -0.5f, 1.0f, 2.0f },
	{ -0.5f, 1.0f, 1.5f },
	{ -0.5f, 1.0f, 1.0f },
	{  0.0f, 1.0f, 1.0f },
	{  0.5f, 1.5f, 1.0f },
	{  0.5f, 1.5f, 1.5f },
	{  0.5f, 1.5f, 2.0f },
	{  0.0f, 1.5f, 2.0f },
	{ -0.5f, 1.5f, 2.0f },
	{ -0.5f, 1.5f, 1.5f },
	{ -0.5f, 1.5f, 1.0f },
	{  0.0f, 1.5f, 1.0f },
	{  0.5f, 2.0f, 1.0f },
	{  0.5f, 2.0f, 1.5f },
	{  0.5f, 2.0f, 2.0f },
	{  0.0f, 2.0f, 2.0f },
	{ -0.5f, 2.0f, 2.0f },
	{ -0.5f, 2.0f, 1.5f },
	{ -0.5f, 2.0f, 1.0f },
	{  0.0f, 2.0f, 1.0f },
	{  0.5f, 2.3f, 1.0f },
	{  0.5f, 2.3f, 1.5f },
	{  0.5f, 2.3f, 2.0f },
	{  0.0f, 2.3f, 2.0f },
	{ -0.5f, 2.3f, 2.0f },
	{ -0.5f, 2.3f, 1.5f },
	{ -0.5f, 2.3f, 1.0f },
	{ -0.5f, 2.0f, 1.0f },
	{ -0.5f, 2.0f, 1.0f },
	{ -0.5f, 0.7f, 0.5f },
	{ -0.5f, 1.8f, 0.0f },
	{ -0.5f, 1.4f, -0.5f },
	{ -0.5f, 2.2f, -1.0f },
	{ -0.5f, 0.3f, -1.5f },
	{ -0.5f, 1.0f, -2.0f },
	{ -0.5f, 1.0f, -2.0f },
	{  0.0f, 1.0f, -1.0f },
	{  0.0f, 1.0f, 0.0f },
	};

	dragon::step = dragon::path.points.size() / (float)dragon::n;

	dragon::normal = std::vector<glm::vec3>(dragon::n, glm::vec3(1.0f));
	dragon::tangent = std::vector<glm::vec3>(dragon::n, glm::vec3(1.0f));
	dragon::bitangent = std::vector<glm::vec3>(dragon::n, glm::vec3(1.0f));

	for (int i = 0; i < dragon::n - 1; i++)
	{
		dragon::tangent[i] = glm::normalize(dragon::path.GetSplinePoint(i * dragon::step + dragon::step, true) - dragon::path.GetSplinePoint(i * dragon::step, true));
		//tangent[i] = glm::normalize(path.GetSplinePoint(i * step, true));
		//std::cout << "i: " << i << " " << tangent[i].x << " " << tangent[i].y << " " << tangent[i].z << std::endl;
	}

	dragon::tangent[dragon::n - 1] = glm::normalize(dragon::path.GetSplinePoint((dragon::n - 1) * dragon::step, true));

	//normal[0] = glm::normalize(glm::cross(tangent[0], glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), tangent[0]))));
	dragon::normal[0] = glm::vec3(0.0f, 1.0f, 0.0f);

	for (int i = 0; i < dragon::n - 1; i++)
	{
		dragon::bitangent[i] = glm::cross(dragon::tangent[i], dragon::tangent[i + 1]);
		//std::cout << "i: " << i << " " << glm::length(bitangent[i]) << std::endl;
		if (fabs(glm::length(dragon::bitangent[i])) < 0.01f)
		{
			dragon::normal[i + 1] = dragon::normal[i];
			dragon::tangent[i + 1] = dragon::tangent[i];
			//std::cout << "i: " << i << " bitangent 0" << std::endl;
		}
		else
		{
			dragon::bitangent[i] = glm::normalize(dragon::bitangent[i]);
			dragon::theta = glm::radians(glm::acos(glm::dot(glm::normalize(dragon::tangent[i]), glm::normalize(dragon::tangent[i + 1]))));
			//glm::mat4 matrix = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
			dragon::normal[i + 1] = glm::vec3(glm::rotate(dragon::theta, dragon::bitangent[i]) * glm::vec4(dragon::normal[i], 0.0f));
			//normal[i + 1] = glm::normalize(glm::rotate(bitangent[i], theta, normal[i]));
		}
		//std::cout << "i: " << i << " normal: " << normal[i].x << " " << normal[i].y << " " << normal[i].z << std::endl;
		//std::cout << "i: " << i << " tangent: " << tangent[i].x << " " << tangent[i].y << " " << tangent[i].z << std::endl;
		//std::cout << "i: " << i << " bitangent: " << bitangent[i].x << " " << bitangent[i].y << " " << bitangent[i].z << std::endl;

		dragon::bitangent[i] = glm::normalize(glm::cross(dragon::normal[i], dragon::tangent[i]));
	}
}

void updateDeltaTime(float time)
{
	if (lastTime < 0) {
		lastTime = time;
		return;
	}

	deltaTime = time - lastTime;
	if (deltaTime > 0.1) deltaTime = 0.1;
	lastTime = time;
}

glm::mat4 createCameraMatrix()
{
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide, cameraDir));
	glm::mat4 cameraRotrationMatrix = glm::mat4({
		cameraSide.x,cameraSide.y,cameraSide.z,0,
		cameraUp.x,cameraUp.y,cameraUp.z ,0,
		-cameraDir.x,-cameraDir.y,-cameraDir.z,0,
		0.,0.,0.,1.,
		});
	cameraRotrationMatrix = glm::transpose(cameraRotrationMatrix);
	glm::mat4 cameraMatrix = cameraRotrationMatrix * glm::translate(-cameraPos);

	return cameraMatrix;
}

glm::mat4 createPerspectiveMatrix()
{

	glm::mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 50.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,aspectRatio,0.,0.,
		0.,0.,(f + n) / (n - f),2 * f * n / (n - f),
		0.,0.,-1.,0.,
		});


	perspectiveMatrix = glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

void drawObjectPBR(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color, float roughness, float metallic)
{

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniform1f(glGetUniformLocation(program, "exposition"), exposition);

	glUniform1f(glGetUniformLocation(program, "roughness"), roughness);
	glUniform1f(glGetUniformLocation(program, "metallic"), metallic);

	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);

	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(program, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
	glUniform3f(glGetUniformLocation(program, "sunColor"), sunColor.x, sunColor.y, sunColor.z);

	glUniform3f(glGetUniformLocation(program, "lightPos"), pointlightPos.x, pointlightPos.y, pointlightPos.z);
	glUniform3f(glGetUniformLocation(program, "lightColor"), pointlightColor.x, pointlightColor.y, pointlightColor.z);

	glUniform3f(glGetUniformLocation(program, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(program, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	glUniform3f(glGetUniformLocation(program, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	glUniform1f(glGetUniformLocation(program, "spotlightPhi"), spotlightPhi);
	Core::DrawContext(context);

}

void renderShadowapSun() {
	float time = glfwGetTime();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//uzupelnij o renderowanie glebokosci do tekstury






	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
}

void renderScene(GLFWwindow* window)
{
	///// SLEEP

	Sleep(10);

	glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float time = glfwGetTime();
	updateDeltaTime(time);
	renderShadowapSun();

	glUseProgram(programSun);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1));
	glUniformMatrix4fv(glGetUniformLocation(programSun, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniform3f(glGetUniformLocation(programSun, "color"), sunColor.x / 2, sunColor.y / 2, sunColor.z / 2);
	glUniform1f(glGetUniformLocation(programSun, "exposition"), exposition);
	Core::DrawContext(sphereContext);
	glUseProgram(program);

	///// DRAGON

	// positions
	glm::vec3 headPosition = dragon::path.GetSplinePoint(dragon::headIndex * dragon::step, true);
	glm::vec3 body1Position = dragon::path.GetSplinePoint(dragon::body1Index * dragon::step, true);
	glm::vec3 body2Position = dragon::path.GetSplinePoint(dragon::body2Index * dragon::step, true);
	glm::vec3 body3Position = dragon::path.GetSplinePoint(dragon::body3Index * dragon::step, true);
	glm::vec3 tailPosition = dragon::path.GetSplinePoint(dragon::tailIndex * dragon::step, true);

	// rotation matrices
	glm::mat4 headRotation = glm::mat4(
		glm::vec4(dragon::bitangent[dragon::headIndex], 0),
		glm::vec4(dragon::normal[dragon::headIndex], 0),
		glm::vec4(dragon::tangent[dragon::headIndex], 0),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 body1Rotation = glm::mat4(
		glm::vec4(dragon::bitangent[dragon::body1Index], 0),
		glm::vec4(dragon::normal[dragon::body1Index], 0),
		glm::vec4(dragon::tangent[dragon::body2Index], 0),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 body2Rotation = glm::mat4(
		glm::vec4(dragon::bitangent[dragon::body2Index], 0),
		glm::vec4(dragon::normal[dragon::body2Index], 0),
		glm::vec4(dragon::tangent[dragon::body2Index], 0),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 body3Rotation = glm::mat4(
		glm::vec4(dragon::bitangent[dragon::body3Index], 0),
		glm::vec4(dragon::normal[dragon::body3Index], 0),
		glm::vec4(dragon::tangent[dragon::body3Index], 0),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 tailRotation = glm::mat4(
		glm::vec4(dragon::bitangent[dragon::tailIndex], 0),
		glm::vec4(dragon::normal[dragon::tailIndex], 0),
		glm::vec4(dragon::tangent[dragon::tailIndex], 0),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);

	// drawing
	if (!dragon::hasBeenShot)
	{
		drawObjectPBR(models::dragonSphere, glm::translate(headPosition) * headRotation * glm::scale(glm::vec3(1.0f)), glm::vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f);
		drawObjectPBR(models::dragonCylinder, glm::translate(body1Position) * body1Rotation * glm::scale(glm::vec3(0.7f)), glm::vec3(0.5f, 0.1f, 0.7f), 0.0f, 0.0f);
		drawObjectPBR(models::dragonCylinder, glm::translate(body2Position) * body2Rotation * glm::scale(glm::vec3(0.7f)), glm::vec3(0.5f, 0.1f, 0.7f), 0.0f, 0.0f);
		drawObjectPBR(models::dragonCylinder, glm::translate(body3Position) * body3Rotation * glm::scale(glm::vec3(0.7f)), glm::vec3(0.5f, 0.1f, 0.7f), 0.0f, 0.0f);
		drawObjectPBR(models::dragonCone, glm::translate(tailPosition) * tailRotation * glm::scale(glm::vec3(0.7f)), glm::vec3(0.5f, 0.1f, 0.7f), 0.0f, 0.0f);
	}

	// index
	dragon::headIndex++; if (dragon::headIndex == dragon::n) dragon::headIndex = 0;
	dragon::body1Index++; if (dragon::body1Index == dragon::n) dragon::body1Index = 0;
	dragon::body2Index++; if (dragon::body2Index == dragon::n) dragon::body2Index = 0;
	dragon::body3Index++; if (dragon::body3Index == dragon::n) dragon::body3Index = 0;
	dragon::tailIndex++; if (dragon::tailIndex == dragon::n) dragon::tailIndex = 0;

	///// ARROW

	if (arrow::hasBeenShot)
	{
		// update
		arrow::time += deltaTime;
		arrow::flySpeed = 0.05f * deltaTime * 60;
		arrow::fallSpeed = 0.01f * deltaTime * 60;
		arrow::position += arrow::direction * arrow::flySpeed;
		arrow::position -= arrow::up * arrow::fallSpeed * arrow::time;

		// wall collision
		float roomMinX = -10.0f;
		float roomMaxX = 10.0f;
		float roomMinY = -10.0f;
		float roomMaxY = 10.0f;
		float roomMinZ = -10.0f;
		float roomMaxZ = 10.0f;
		if (fabs(roomMinX - arrow::position.x) < arrow::radius ||
			fabs(roomMaxX - arrow::position.x) < arrow::radius ||
			fabs(roomMinY - arrow::position.y) < arrow::radius ||
			fabs(roomMaxY - arrow::position.y) < arrow::radius ||
			fabs(roomMinZ - arrow::position.z) < arrow::radius ||
			fabs(roomMaxZ - arrow::position.z) < arrow::radius
			)
		{
			arrow::hasBeenShot = false;
		}

		// column collision
		float columnRadius = 1.0f;

		// torch collision
		float torchRadius = 1.0f;
		float torchMinY = 1.0f;
		float torchMaxY = 1.0f;

		// dragon center body part collision
		float distance = sqrt(
			(body2Position.x - arrow::position.x) * (body2Position.x - arrow::position.x) +
			(body2Position.y - arrow::position.y) * (body2Position.y - arrow::position.y) +
			(body2Position.z - arrow::position.z) * (body2Position.z - arrow::position.z)
		);
		if (distance < arrow::radius)
		{
			dragon::hasBeenShot = true;
		}

		// rotation matrix
		arrow::side = glm::normalize(glm::cross(arrow::direction, glm::vec3(0.f, 1.f, 0.f)));
		arrow::up = glm::normalize(glm::cross(arrow::side, arrow::direction));
		arrow::cameraRotrationMatrix = glm::mat4({
			arrow::side.x,arrow::side.y,arrow::side.z,0,
			arrow::up.x,arrow::up.y,arrow::up.z ,0,
			-arrow::direction.x,-arrow::direction.y,-arrow::direction.z,0,
			0.,0.,0.,1.,
			});

		// draw
		drawObjectPBR(shipContext,
			glm::translate(arrow::position) * arrow::cameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.01f)),
			glm::vec3(0.1, 0.9, 0.9),
			0.2, 1.0
		);
	}

	///// SHIP

	// rotation matrix
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});

	// color
	glm::vec3 shipColor = glm::vec3(0.1, 0.9, 0.1);
	if (arrow::hasBeenShot) shipColor = glm::vec3(0.9, 0.1, 0.1);
	if (dragon::hasBeenShot) shipColor = glm::vec3(0.9, 0.9, 0.1);

	// drawing
	drawObjectPBR(shipContext,
		glm::translate(spaceshipPos) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.03f)),
		shipColor, 0.2, 1.0
	);

	// update
	spotlightPos = spaceshipPos + 0.2 * spaceshipDir;
	spotlightConeDir = spaceshipDir;

	///// SCENE DRAWING

	drawObjectPBR(models::columnMiddleContext, glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	drawObjectPBR(models::columnNEContext, glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	drawObjectPBR(models::columnNWContext, glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	drawObjectPBR(models::columnSEContext, glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	drawObjectPBR(models::columnSWContext, glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	drawObjectPBR(models::doorLeftContext, glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	drawObjectPBR(models::doorRightContext, glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	drawObjectPBR(models::torchEContext, glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	drawObjectPBR(models::torchNEContext, glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	drawObjectPBR(models::torchNWContext, glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	drawObjectPBR(models::torchSEContext, glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	drawObjectPBR(models::torchSWContext, glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	drawObjectPBR(models::torchWContext, glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	drawObjectPBR(models::roomContext, glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	drawObjectPBR(models::room, glm::mat4(), glm::vec3(0.9f, 0.9f, 0.9f), 0.8f, 0.0f);

	//test depth buffer
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUseProgram(programTest);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	//Core::DrawContext(models::testContext);

	glUseProgram(0);
	glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
}

void loadModelToContext(std::string path, Core::RenderContext& context)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}

void init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader_9_1.vert", "shaders/shader_9_1.frag");
	programTest = shaderLoader.CreateProgram("shaders/test.vert", "shaders/test.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_8_sun.vert", "shaders/shader_8_sun.frag");

	loadModelToContext("./models/spaceship.obj", shipContext);

	loadModelToContext("./models/room.obj", models::roomContext);
	loadModelToContext("./models/spaceship.obj", models::spaceshipContext);



	loadModelToContext("./models/columnMiddle.obj", models::columnMiddleContext);
	loadModelToContext("./models/columnNE.obj", models::columnNEContext);
	loadModelToContext("./models/columnNW.obj", models::columnNWContext);
	loadModelToContext("./models/columnSE.obj", models::columnSEContext);
	loadModelToContext("./models/columnSW.obj", models::columnSWContext);
	loadModelToContext("./models/doorLeft.obj", models::doorLeftContext);
	loadModelToContext("./models/doorRight.obj", models::doorRightContext);
	loadModelToContext("./models/torchE.obj", models::torchEContext);
	loadModelToContext("./models/torchNE.obj", models::torchNEContext);
	loadModelToContext("./models/torchNW.obj", models::torchNWContext);
	loadModelToContext("./models/torchSE.obj", models::torchSEContext);
	loadModelToContext("./models/torchSW.obj", models::torchSWContext);
	loadModelToContext("./models/torchW.obj", models::torchWContext);


	loadModelToContext("./models/dragonSphere.obj", models::dragonSphere);
	loadModelToContext("./models/dragonCylinder.obj", models::dragonCylinder);
	loadModelToContext("./models/dragonCone.obj", models::dragonCone);

	initDragon();
}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::vec3(0.f, 1.f, 0.f);
	float angleSpeed = 0.05f * deltaTime * 60;
	float moveSpeed = 0.05f * deltaTime * 60;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		spaceshipPos += spaceshipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		spaceshipPos -= spaceshipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		spaceshipPos += spaceshipSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		spaceshipPos -= spaceshipSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		spaceshipPos += spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		spaceshipPos -= spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(spaceshipDir, 0));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(spaceshipDir, 0));

	cameraPos = spaceshipPos - 0.5 * spaceshipDir + glm::vec3(0, 1, 0) * 0.2f;
	cameraDir = spaceshipDir;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		exposition -= 0.05;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		exposition += 0.05;

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		printf("spaceshipPos = glm::vec3(%ff, %ff, %ff);\n", spaceshipPos.x, spaceshipPos.y, spaceshipPos.z);
		printf("spaceshipDir = glm::vec3(%ff, %ff, %ff);\n", spaceshipDir.x, spaceshipDir.y, spaceshipDir.z);
		printf("arrow::position = glm::vec3(%ff, %ff, %ff);\n", arrow::position.x, arrow::position.y, arrow::position.z);
		printf("arrow::direction = glm::vec3(%ff, %ff, %ff);\n", arrow::direction.x, arrow::direction.y, arrow::direction.z);
	}

	//cameraDir = glm::normalize(-cameraPos);

	///// ARROW

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		//if (!arrow::hasBeenShot)
		{
			arrow::hasBeenShot = true;
			arrow::time = 0.0f;
			arrow::position = spaceshipPos;
			arrow::direction = spaceshipDir;
		}
	}
}

// funkcja jest glowna petla
void renderLoop(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		renderScene(window);
		glfwPollEvents();
	}
}
//}