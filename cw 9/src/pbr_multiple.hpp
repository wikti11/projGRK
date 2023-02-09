#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"

#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>


namespace texture {
	GLuint ship;

	GLuint shipAlbedo;
	GLuint shipNormal;
	GLuint shipAMR;

	GLuint p6Albedo;
	GLuint p6Normal;
	GLuint p6ARM;
	GLuint p6Depth;

	GLuint floorA;
	GLuint floorN;
	GLuint floorAO;
	GLuint floorR;
	GLuint floorM;
	GLuint floorD;
}

namespace models {
	Core::RenderContext shipContext;
	Core::RenderContext sphereContext;
	Core::RenderContext wallContext;
	Core::RenderContext floorContext;
}


GLuint program;
GLuint programSun;
Core::Shader_Loader shaderLoader;

glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);

glm::vec3 spaceshipPos = glm::vec3(-4.f, 0, 0);
glm::vec3 spaceshipDir = glm::vec3(1.f, 0.f, 0.f);

GLuint VAO,VBO;

float aspectRatio = 1.f;

float exposition = 1.f;

glm::vec3 lightColor = glm::vec3(0.9, 0.7, 0.8)*100;

// Lista pozycji oświetlenia (źródeł światła)
glm::vec3 lightPositions[] = {
	glm::vec3(0.0f, 0.0f, 0.0f), // 1
	glm::vec3(25.0f, 0.0f, 0.0f), // 2
	glm::vec3(10.0f, 10.0f, 0.0f), // 3
	glm::vec3(-20.0f, 0.0f, 15.0f), // 4
	glm::vec3(14.0f, 0.0f, 5.0f), // 5
	glm::vec3(-14.0f, 0.0f, 10.0f), // 6
	glm::vec3(4.0f, 20.0f, 15.0f), // 7
	glm::vec3(-40.0f, 0.0f, 30.0f), // 8
	glm::vec3(50.0f, 0.0f, 0.0f), // 9
	glm::vec3(50.0f, 0.0f, 50.0f), // 10
};

// Lista kolorów oświetlenia
glm::vec3 lightColors[] = {
	lightColor, // 1
	lightColor, // 2
	lightColor, // 3
	lightColor, // 4
	lightColor, // 5
	lightColor, // 6
	lightColor, // 7
	lightColor, // 8
	lightColor, // 9
	lightColor, // 10
};



glm::vec3 spotlightPos = glm::vec3(0, 0, 0);
glm::vec3 spotlightConeDir = glm::vec3(0, 0, 0);
glm::vec3 spotlightColor = glm::vec3(0.5, 0.9, 0.8)*10;
float spotlightPhi = 3.14 / 3;


float lastTime = -1.f;
float deltaTime = 0.f;

void updateDeltaTime(float time) {
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
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir,glm::vec3(0.f,1.f,0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide,cameraDir));
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
	float f = 1000.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,aspectRatio,0.,0.,
		0.,0.,(f+n) / (n - f),2*f * n / (n - f),
		0.,0.,-1.,0.,
		});

	
	perspectiveMatrix=glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

void drawObjectColor(GLuint &program, Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color) {
	
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniform1f(glGetUniformLocation(program, "exposition"), exposition);

	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);

	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);

	Core::DrawContext(context);

}

// PBR + tekstury + normal mapping
void drawObjectTexture(GLuint &program, Core::RenderContext& context, glm::mat4 modelMatrix, GLuint AlbedoMap, GLuint NormalMap, GLuint AmbientOcclusionMap, GLuint RoughnessMap, GLuint MetalnessMap) {

	glUseProgram(program);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniform1f(glGetUniformLocation(program, "exposition"), exposition);

	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	for (int i=0; i < 10; i++) {
		std::string number = std::to_string(i);
		glUniform3f(glGetUniformLocation(program, ("pointLights[" + number + "]").c_str()), lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
		glUniform3f(glGetUniformLocation(program, ("pointLightsColors[" + number + "]").c_str()), lightColors[i].x, lightColors[i].y, lightColors[i].z);
	}

	Core::SetActiveTexture(AlbedoMap, "albedoMap", program, 0);
	Core::SetActiveTexture(NormalMap, "normalMap", program, 1);
	Core::SetActiveTexture(AmbientOcclusionMap, "aoMap", program, 2);
	Core::SetActiveTexture(RoughnessMap, "roughMap", program, 3);
	Core::SetActiveTexture(MetalnessMap, "metalMap", program, 4);
	Core::DrawContext(context);

}

void renderScene(GLFWwindow* window)
{
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 transformation;
	float time = glfwGetTime();
	updateDeltaTime(time);

	for (int i = 0; i < 10; i++) {
		glUseProgram(programSun);
		transformation = createPerspectiveMatrix() * createCameraMatrix() * glm::translate(lightPositions[i]);
		glUniformMatrix4fv(glGetUniformLocation(programSun, "transformation"), 1, GL_FALSE, (float*)&transformation);
		glUniform3f(glGetUniformLocation(programSun, "color"), 0.9f, 0.8f, 0.4f);
		Core::DrawContext(models::sphereContext);
	}

	glUseProgram(program);

	glm::mat4 modelF = glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f)) * glm::translate(glm::vec3(0.0f, 0.0f, -10.0f)) * glm::scale(glm::vec3(0.25f, 0.25f, 0.25f));

	drawObjectTexture(program, models::floorContext, modelF, texture::floorA, texture::floorN, texture::floorAO, texture::floorR, texture::floorM);

	modelF = glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f)) * glm::translate(glm::vec3(0.0f, 0.0f, 3.0f)) * glm::scale(glm::vec3(0.25f, 0.25f, 0.25f));

	drawObjectTexture(program, models::floorContext, modelF, texture::floorA, texture::floorN, texture::floorAO, texture::floorR, texture::floorM);

	glUseProgram(program);
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});


	drawObjectTexture(program, models::shipContext,
		glm::translate(spaceshipPos) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>())*glm::scale(glm::vec3(0.1)),
		texture::shipAlbedo, texture::shipNormal, texture::shipAMR, texture::shipAMR, texture::shipAMR);

	spotlightPos = spaceshipPos + 0.5 * spaceshipDir;
	spotlightConeDir = spaceshipDir;

	glUseProgram(0);
	glfwSwapBuffers(window);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
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
	
	glEnable(GL_NORMALIZE);
	program = shaderLoader.CreateProgram("shaders/shader_PBR.vert", "shaders/shader_PBR.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_8_sun.vert", "shaders/shader_8_sun.frag");

	texture::shipAlbedo = Core::LoadTexture("textures/spaceshipPBR/StarSparrow_albedo.png");
	texture::shipNormal = Core::LoadTexture("textures/spaceshipPBR/StarSparrow_Normal.png");
	texture::shipAMR = Core::LoadTexture("textures/spaceshipPBR/StarSparrow_AMR.png");

	texture::p6Albedo = Core::LoadTexture("textures/p6/albedo.jpg");
	texture::p6Depth = Core::LoadTexture("textures/p6/depth.jpg");
	texture::p6Normal = Core::LoadTexture("textures/p6/normal.jpg");
	texture::p6ARM = Core::LoadTexture("textures/p6/arm.jpg");


	texture::floorA = Core::LoadTexture("textures/floor/albedo.png");
	texture::floorN = Core::LoadTexture("textures/floor/normal.png");
	texture::floorAO = Core::LoadTexture("textures/floor/ao.png");
	texture::floorM = Core::LoadTexture("textures/floor/metalness.png");
	texture::floorR = Core::LoadTexture("textures/floor/roughness.png");
	texture::floorD = Core::LoadTexture("textures/floor/height.png");

	loadModelToContext("./models/sphere.obj", models::sphereContext);
	loadModelToContext("./models/spaceship.obj", models::shipContext);
	loadModelToContext("./models/Floor.FBX", models::floorContext);

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
	float moveSpeed = 0.05f * deltaTime * 360;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
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

	cameraPos = spaceshipPos - 1.5 * spaceshipDir + glm::vec3(0, 1, 0) * 0.5f;
	cameraDir = spaceshipDir;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		exposition -= 0.05;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		exposition += 0.05;

	//cameraDir = glm::normalize(-cameraPos);

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