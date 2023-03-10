#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <fstream> 
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"
#include "Spline.cpp"
#include "SOIL/SOIL.h"
#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

int WIDTH = 500, HEIGHT = 500;

namespace models
{
	Core::RenderContext columnContext;
	Core::RenderContext doorLeftContext;
	Core::RenderContext doorRightContext;
	Core::RenderContext roomContext;
	Core::RenderContext torchContext;
	Core::RenderContext torchRingsContext;
	Core::RenderContext torchHandleContext;
	Core::RenderContext room;
	Core::RenderContext dragonSphere;
	Core::RenderContext	dragonCone;
	Core::RenderContext dragonCylinder;
	Core::RenderContext lampContext;
	Core::RenderContext lightSwitchContext;
	Core::RenderContext arrowContext;
	Core::RenderContext crossbowContext;
	Core::RenderContext sphereContext;
	Core::RenderContext fireContext;
}

namespace crossbowTexture
{
	GLuint albedo;
	GLuint normal;
	GLuint ambientOcclusion;
	GLuint roughness;
	GLuint metalness;
}

namespace columnTexture
{
	GLuint albedo;
	GLuint normal;
	GLuint ambientOcclusion;
	GLuint roughness;
	GLuint metalness;
}

namespace destroyedColumnTexture
{
	GLuint albedo;
	GLuint normal;
	GLuint ambientOcclusion;
	GLuint roughness;
	GLuint metalness;
}

namespace roomTexture
{
	GLuint albedo;
	GLuint normal;
	GLuint ambientOcclusion;
	GLuint roughness;
	GLuint metalness;
}

namespace torchTexture
{
	GLuint albedo;
	GLuint normal;
	GLuint ambientOcclusion;
	GLuint roughness;
	GLuint metalness;
}

namespace torchHandleTexture
{
	GLuint albedo;
	GLuint normal;
	GLuint ambientOcclusion;
	GLuint roughness;
	GLuint metalness;
}

namespace torchRingsTexture
{
	GLuint albedo;
	GLuint normal;
	GLuint ambientOcclusion;
	GLuint roughness;
	GLuint metalness;
}

namespace arrowTexture
{
	GLuint albedo;
	GLuint normal;
	GLuint ambientOcclusion;
	GLuint roughness;
	GLuint metalness;
}

namespace lampTexture
{
	GLuint albedo;
	GLuint normal;
	GLuint ambientOcclusion;
	GLuint roughness;
	GLuint metalness;
}


namespace lightSwitchTexture
{
	GLuint albedo;
	GLuint normal;
	GLuint ambientOcclusion;
	GLuint roughness;
	GLuint metalness;
}

namespace doorTexture
{
	GLuint albedo;
	GLuint normal;
	GLuint ambientOcclusion;
	GLuint roughness;
	GLuint metalness;
}

GLuint depthMapFBO;
GLuint depthMap;
GLuint cubemapTexture;
GLuint programSkybox;
GLuint program;
GLuint programSun;
GLuint programTest;
GLuint programTex;
GLuint programPBR;

Core::Shader_Loader shaderLoader;
Core::RenderContext skyboxContext;


glm::vec3 sunPos = glm::vec3(-4.740971f, 2.149999f, 0.369280f);
glm::vec3 sunDir = glm::vec3(-0.93633f, 0.351106, 0.003226f);
glm::vec3 sunColor = glm::vec3(0.9f, 0.9f, 0.7f) * 5;

glm::vec3 cameraPos = glm::vec3(0.479490f, 1.250000f, -2.124680f);
glm::vec3 cameraDir = glm::vec3(-0.354510f, 0.000000f, 0.935054f);

//glm::vec3 crossbowPos = glm::vec3(0.065808f, 1.250000f, -2.189549f);
//glm::vec3 crossbowDir = glm::vec3(-0.490263f, 0.000000f, 0.871578f);
glm::vec3 crossbowPos = glm::vec3(-9.0f, 1.25f, -9.0f);
glm::vec3 crossbowDir = glm::vec3(0.75f, 0.0f, 0.75f);
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
bool shot = false;

std::vector<glm::vec3> columnPosition = {
	{0.0f, 0.0f, 0.0f},
	{-4.f, 0.0f, -6.0f},
	{-4.f, 0.0f, 6.0f},
	{4.f, 0.0f, -6.0f},
	{4.f, 0.0f, 6.0f},
};
std::vector<bool> columnDamaged = std::vector<bool>(columnPosition.size(), false);
std::vector<bool> columnDestroyed = std::vector<bool>(columnPosition.size(), false);

std::vector<glm::vec3> torchPosition = {
	{ -9.57197f, 2.219f, 8.08489f },
	{-9.57197f, 2.219f, -8.08489f },
	{ 0.0f, 2.219f, 9.81309f },
	{ 0.0f, 2.219f, -10.0318f },
	{ 10.2736f, 2.219f, 8.08489f },
	{ 10.2736f, 2.219f, -8.08489f },
};
std::vector<float> torchAngle = {
	0.0f,
	0.0f,
	90.0f,
	-90.0f,
	180.0f,
	180.0f,
};
std::vector<bool> torchExisting = std::vector<bool>(torchPosition.size(), true);

std::vector<glm::vec3> lampPosition = {
	{ -9.57197f, 2.219f, 3.81309f },
	{ -9.57197f, 2.219f, -3.0318f },
	{ 10.2736f, 2.219f, -3.08489f },
	{ 10.2736f, 2.219f, 3.08489f },
};
std::vector<float> lampAngle = {
	90.0f,
	90.0f,
	-90.0f,
	-90.0f,
};
std::vector<bool> lampSwitchedOn = std::vector<bool>(torchPosition.size(), false);

std::vector<glm::vec3> switchPosition = {
	{ -9.57197f, 2.219f, 5.08489f },
	{ -9.57197f, 2.219f, -5.08489f },
	{ 10.2736f, 2.219f, -5.08489f },
	{ 10.2736f, 2.219f, 5.08489f },
};
std::vector<float> switchAngle = {
	-90.0f,
	-90.0f,
	90.0f,
	90.0f,
};

std::vector<glm::vec3> firePosition = {
	{ -9.07197f, 3.519f, 8.13489f },
	{-9.07197f, 3.519f, -8.03489f },
	{ 0.07f, 3.519f, 9.41309f },
	{ 0.07f, 3.519f, -9.4318f },
	{ 9.7736f, 3.519f, 8.15489f },
	{ 9.7736f, 3.519f, -8.03489f },
};

std::vector<glm::vec3> lightPositions = {
	firePosition[0],
	firePosition[1],
	firePosition[2],
	firePosition[3],
	firePosition[4],
	firePosition[5],
	lampPosition[0],
	lampPosition[1],
	lampPosition[2],
	lampPosition[3],
};

glm::vec3 lightColor = glm::vec3(0.9, 0.7, 0.8);

std::vector<glm::vec3> lightColors = {
	lightColor,
	lightColor,
	lightColor,
	lightColor,
	lightColor,
	lightColor,
	lightColor,
	lightColor,
	lightColor,
	lightColor,
};

// particles

struct Particle
{
	glm::vec3 Position, Velocity;
	glm::vec3 Color;
	float Life;

	Particle()
		: Position(0.0f), Velocity(0.0f), Color(0.0f), Life(0.0f) {}
};

GLuint fireTexture;

namespace fire {
	std::vector<Particle> particles;
	int lastUsedParticle = 0;
	int nr_particles = 1500;
}

void initParticles() {
	for (unsigned int i = 0; i < fire::nr_particles; ++i)
		fire::particles.push_back(Particle());
};

int firstUnusedParticle()
{
	for (int i = fire::lastUsedParticle; i < fire::nr_particles; ++i) {
		if (fire::particles[i].Life <= 0.0f) {
			fire::lastUsedParticle = i;
			return i;
		}
	}
	for (unsigned int i = 0; i < fire::lastUsedParticle; ++i) {
		if (fire::particles[i].Life <= 0.0f) {
			fire::lastUsedParticle = i;
			return i;
		}
	}
	fire::lastUsedParticle = 0;
	return 0;
}

void respawnParticle(Particle& particle, glm::vec3 offset, glm::vec3 firePos)
{
	float randomx = ((rand() % 100) - 50) / 300.0f;
	float randomy = ((rand() % 100) - 50) / 300.0f;
	float randomz = ((rand() % 100) - 50) / 300.0f;
	glm::vec3 Color1 = glm::vec3(0.98f, 0.24f, 0.05f);
	glm::vec3 Color2 = glm::vec3(0.98f, 0.47f, 0.05f);
	glm::vec3 Color3 = glm::vec3(1.00f, 0.78f, 0.00f);
	particle.Position = firePos + glm::vec3(randomx, randomy, randomz);
	int choice = (rand() % 3) + 1;
	if (choice == 1)
		particle.Color = Color1;
	if (choice == 2)
		particle.Color = Color2;
	if (choice == 3)
		particle.Color = Color3;
	if (choice < 3)
		float randomy = ((rand() % 100) - 80) / 300.0f;
	particle.Life = 1.0f;
	particle.Velocity = glm::vec3(0.1f);
}

void particleUpdate(float dt, int newParticles, glm::vec3 offset, glm::vec3 firePos)
{
	for (int i = 0; i < newParticles; ++i)
	{
		int unusedParticle = firstUnusedParticle();
		respawnParticle(fire::particles[unusedParticle], offset, firePos);
	}
	for (int i = 0; i < fire::nr_particles; ++i)
	{
		Particle& p = fire::particles[i];
		p.Life -= dt;
		if (p.Life > 0.0f)
		{
			p.Position -= p.Velocity * dt;
		}
	}
}

namespace dragon
{
	bool hasBeenShot = false;

	std::vector<glm::vec3> path1 = std::vector<glm::vec3>(50, glm::vec3(0.0f, 2.0f, 0.0f));
	std::vector<glm::vec3> path2 = std::vector<glm::vec3>(50, glm::vec3(0.0f, 2.0f, 0.0f));
	std::vector<glm::vec3> path3 = std::vector<glm::vec3>(50, glm::vec3(0.0f, 2.0f, 0.0f));
	int currentPath = 1;
	int currentEditedPath = 1;
	bool isEditingPath = false;
	int editedPointIndex = 0;

	Spline spline;
	int n = 1000;
	float step = 0.0f;
	float theta = 0.0f;

	std::vector<glm::vec3> normal;
	std::vector<glm::vec3> tangent;
	std::vector<glm::vec3> bitangent;

	int headIndex = 42;
	std::vector<int> bodyIndex = { 38,36,34,32,30,28,26,24,22,20,18,16,14,12,10,8,6,4,2 };
	//std::vector<int> bodyIndex = {19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};
	int tailIndex = 0;

	bool isFalling = false;
	float fallSpeed = 1.0f;
	float fallTime = 1.0f;
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
	float flySpeed = 2.0f;
	float fallSpeed = 1.0f;
	bool hasBeenShot = false;
}

void loadPathsFromFile();
void savePathsToFile();
void loadDeltaTimeFromFile();
void saveDeltaTimeToFile();

void initPath()
{
	dragon::step = dragon::spline.points.size() / (float)dragon::n;

	dragon::normal = std::vector<glm::vec3>(dragon::n, glm::vec3(1.0f));
	dragon::tangent = std::vector<glm::vec3>(dragon::n, glm::vec3(1.0f));
	dragon::bitangent = std::vector<glm::vec3>(dragon::n, glm::vec3(1.0f));

	for (int i = 0; i < dragon::n - 1; i++)
	{
		//dragon::tangent[i] = glm::normalize(dragon::spline.GetSplinePoint(i * dragon::step + dragon::step, true) - dragon::spline.GetSplinePoint(i * dragon::step, true));
		dragon::tangent[i] = glm::normalize(dragon::spline.GetSplineGradient(i * dragon::step + dragon::step, true));
		//tangent[i] = glm::normalize(path.GetSplinePoint(i * step, true));
		//std::cout << "i: " << i << " " << tangent[i].x << " " << tangent[i].y << " " << tangent[i].z << std::endl;
	}

	dragon::tangent[dragon::n - 1] = glm::normalize(dragon::spline.GetSplinePoint((dragon::n - 1) * dragon::step, true));

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

glm::mat4 gram_schmidt_orthonormalization(glm::mat4 vMatrix)
{
	vMatrix = glm::transpose(vMatrix);
	glm::mat4 uMatrix = glm::mat4(0.0f);
	uMatrix[0] = glm::normalize(vMatrix[0]);
	for (int i = 1; i < 4; i++)
	{
		uMatrix[i] = vMatrix[i];
		for (int j = 0; j < i; j++)
		{
			uMatrix[i] = uMatrix[i] - glm::dot(uMatrix[j], uMatrix[i]) * uMatrix[j];
		}
		uMatrix[i] = glm::normalize(uMatrix[i]);
	}
	uMatrix = glm::transpose(uMatrix);
	return uMatrix;
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

// PBR + tekstury + normal mapping
void drawObjectTexture(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint AlbedoMap, GLuint NormalMap, GLuint AmbientOcclusionMap, GLuint RoughnessMap, GLuint MetalnessMap)
{
	glUseProgram(programPBR);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programPBR, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programPBR, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform1f(glGetUniformLocation(programPBR, "exposition"), exposition);
	glUniform3f(glGetUniformLocation(programPBR, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	for (int i = 0; i < 10; i++)
	{
		std::string number = std::to_string(i);
		glUniform3f(glGetUniformLocation(programPBR, ("pointLights[" + number + "]").c_str()), lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
		float c = 100.0f;
		if (i < 6 && !torchExisting[i]) c = 0.0f;
		if (i >= 6 && !lampSwitchedOn[i - 6]) c = 0.0f;
		glUniform3f(glGetUniformLocation(programPBR, ("pointLightsColors[" + number + "]").c_str()), lightColors[i].x * c, lightColors[i].y * c, lightColors[i].z * c);
	}

	Core::SetActiveTexture(AlbedoMap, "albedoMap", programPBR, 0);
	Core::SetActiveTexture(NormalMap, "normalMap", programPBR, 1);
	Core::SetActiveTexture(AmbientOcclusionMap, "aoMap", programPBR, 2);
	Core::SetActiveTexture(RoughnessMap, "roughMap", programPBR, 3);
	Core::SetActiveTexture(MetalnessMap, "metalMap", programPBR, 4);
	Core::DrawContext(context);
}

void drawObjectPBR(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color, float roughness, float metallic)
{
	glUseProgram(program);
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

void loadCubemap(char* cubemap[])
{
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	int w, h;
	unsigned char* data;
	for (unsigned int i = 0; i < 6; i++)
	{
		data = SOIL_load_image(cubemap[i], &w, &h, 0, SOIL_LOAD_RGBA);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void drawSkybox(glm::mat4 modelMatrix) {
	glDisable(GL_DEPTH_TEST);
	glUseProgram(programSkybox);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "transformation"), 1, GL_FALSE, (float*)&transformation);
	Core::SetActiveTexture(cubemapTexture, "skybox", programSkybox, 0);
	Core::DrawContext(skyboxContext);
	glEnable(GL_DEPTH_TEST);
}


// torch consists of 3 main elements so everything is put here to simplify it
// they don't rotate no matter if i use matrix, glm::rotate or glm::eulerAngle
void drawTorch(glm::mat4 translationMatrix, float angle)
{

	//drawObjectPBR(models::torchContext, glm::mat4() * translationMatrix * glm::eulerAngleY(glm::radians(angle)), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	//drawObjectPBR(models::torchRingsContext, glm::mat4() * translationMatrix * glm::eulerAngleY(glm::radians(angle)), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	//drawObjectPBR(models::torchHandleContext, glm::mat4() * translationMatrix * glm::eulerAngleY(glm::radians(angle)), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);

	drawObjectTexture(models::torchContext, glm::mat4() * translationMatrix * glm::eulerAngleY(glm::radians(angle)),
		torchTexture::albedo, torchTexture::normal, torchTexture::ambientOcclusion, torchTexture::roughness, torchTexture::metalness);
	drawObjectTexture(models::torchRingsContext, glm::mat4() * translationMatrix * glm::eulerAngleY(glm::radians(angle)),
		torchRingsTexture::albedo, torchRingsTexture::normal, torchRingsTexture::ambientOcclusion, torchRingsTexture::roughness, torchRingsTexture::metalness);
	drawObjectTexture(models::torchHandleContext, glm::mat4() * translationMatrix * glm::eulerAngleY(glm::radians(angle)),
		torchHandleTexture::albedo, torchHandleTexture::normal, torchHandleTexture::ambientOcclusion, torchHandleTexture::roughness, torchHandleTexture::metalness);
}

void renderScene(GLFWwindow* window)
{
	///// SLEEP
	//Sleep(20);



	glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float time = glfwGetTime();
	updateDeltaTime(time);
	renderShadowapSun();
	drawSkybox(glm::translate(cameraPos));

	glUseProgram(programSun);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1));
	glUniformMatrix4fv(glGetUniformLocation(programSun, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniform3f(glGetUniformLocation(programSun, "color"), sunColor.x / 2, sunColor.y / 2, sunColor.z / 2);
	glUniform1f(glGetUniformLocation(programSun, "exposition"), exposition);
	//Core::DrawContext(sphereContext);
	glUseProgram(program);

	///// DRAGON

	// positions
	glm::vec3 headPosition = dragon::spline.GetSplinePoint(dragon::headIndex * dragon::step, true);
	std::vector<glm::vec3> bodyPosition = std::vector<glm::vec3>(19, glm::vec3());
	for (int i = 0; i < bodyPosition.size(); i++)
	{
		bodyPosition[i] = dragon::spline.GetSplinePoint(dragon::bodyIndex[i] * dragon::step, true);
	}
	glm::vec3 tailPosition = dragon::spline.GetSplinePoint(dragon::tailIndex * dragon::step, true);

	// falling
	if (dragon::isFalling)
	{
		dragon::fallSpeed = 1.0f;
		dragon::fallTime += deltaTime;
		headPosition.y -= dragon::fallTime * dragon::fallTime * dragon::fallSpeed;
		if (dragon::fallTime > 1.0f)
		{
			dragon::isFalling = false;
		}
	}

	// rotation matrices
	glm::mat4 headRotation = glm::mat4(
		glm::vec4(dragon::bitangent[dragon::headIndex], 0),
		glm::vec4(dragon::normal[dragon::headIndex], 0),
		glm::vec4(dragon::tangent[dragon::headIndex], 0),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	headRotation = gram_schmidt_orthonormalization(headRotation);
	std::vector<glm::mat4> bodyRotation = std::vector<glm::mat4>(19, glm::mat4());
	for (int i = 0; i < bodyRotation.size(); i++)
	{
		bodyRotation[i] = glm::mat4(
			glm::vec4(dragon::bitangent[dragon::bodyIndex[i]], 0),
			glm::vec4(dragon::normal[dragon::bodyIndex[i]], 0),
			glm::vec4(dragon::tangent[dragon::bodyIndex[i]], 0),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
		);
		bodyRotation[i] = gram_schmidt_orthonormalization(bodyRotation[i]);
	}
	glm::mat4 tailRotation = glm::mat4(
		glm::vec4(dragon::bitangent[dragon::tailIndex], 0),
		glm::vec4(dragon::normal[dragon::tailIndex], 0),
		glm::vec4(dragon::tangent[dragon::tailIndex], 0),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	tailRotation = gram_schmidt_orthonormalization(tailRotation);

	// drawing
	if (!dragon::isEditingPath && !(dragon::hasBeenShot && !dragon::isFalling))
	{
		drawObjectPBR(models::dragonSphere, glm::translate(headPosition) * headRotation * glm::scale(glm::vec3(1.0f)), glm::vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f);
		for (int i = 0; i < bodyRotation.size(); i++)
		{
			drawObjectPBR(models::dragonCylinder, glm::translate(bodyPosition[i]) * bodyRotation[i] * glm::scale(glm::vec3(0.9f)), glm::vec3(0.5f, 0.1f, 0.7f), 0.0f, 0.0f);
		}
		drawObjectPBR(models::dragonCone, glm::translate(tailPosition) * tailRotation * glm::scale(glm::vec3(0.7f)), glm::vec3(0.5f, 0.1f, 0.7f), 0.0f, 0.0f);
	}

	// index
	if (!dragon::isFalling)
	{
		dragon::headIndex++; if (dragon::headIndex == dragon::n) dragon::headIndex = 0;
		for (int i = 0; i < dragon::bodyIndex.size(); i++)
		{
			dragon::bodyIndex[i]++; if (dragon::bodyIndex[i] == dragon::n) dragon::bodyIndex[i] = 0;
		}
		dragon::tailIndex++; if (dragon::tailIndex == dragon::n) dragon::tailIndex = 0;
	}

	///// ARROW

	if (arrow::hasBeenShot)
	{
		// update
		arrow::time += deltaTime;
		arrow::flySpeed = 0.5f * deltaTime * 60;
		arrow::fallSpeed = 0.01f * deltaTime * 60;
		arrow::position += arrow::direction * arrow::flySpeed;
		arrow::position -= arrow::up * arrow::fallSpeed * arrow::time;

		// wall collision
		float roomMinX = -19.993f / 2;
		float roomMaxX = 19.993f / 2;
		float roomMinZ = -19.993f / 2;
		float roomMaxZ = 19.993f / 2;
		float roomMinY = 0.0f;
		float roomMaxY = 3.98678f;
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
		float columnRadius = 0.747522f;
		for (int i = 0; i < columnPosition.size(); i++)
		{
			glm::vec3 columnPos = columnPosition[i];
			float distance = sqrt(
				(columnPos.x - arrow::position.x) * (columnPos.x - arrow::position.x) +
				(columnPos.z - arrow::position.z) * (columnPos.z - arrow::position.z)
			);
			if (distance < columnRadius + arrow::radius)
			{
				arrow::hasBeenShot = false;
				if (columnDamaged[i])
				{
					columnDestroyed[i] = true;
				}
				else
				{
					columnDamaged[i] = true;
				}
			}
		}

		// torch collision
		float torchRadius = 0.595796f;
		float torchMinY = 1.15648f;
		float torchMaxY = 3.1608f;
		for (int i = 0; i < torchPosition.size(); i++)
		{
			if (!torchExisting[i]) continue;
			glm::vec3 torchPos = torchPosition[i];
			float distance = sqrt(
				(torchPos.x - arrow::position.x) * (torchPos.x - arrow::position.x) +
				(torchPos.z - arrow::position.z) * (torchPos.z - arrow::position.z)
			);
			if (distance < torchRadius + arrow::radius /*&& arrow::position.y > torchMinY && arrow::position.y < torchMaxY*/)
			{
				arrow::hasBeenShot = false;
				torchExisting[i] = false;
			}
		}

		//light switch dimensions taken from blender
		float switchRadius = 0.25f;
		float switchMinY = 1.5f;
		float switchMaxY = 2.5f;
		for (int i = 0; i < switchPosition.size(); i++)
		{
			glm::vec3 switchPos = switchPosition[i];
			float distance = sqrt(
				(switchPos.x - arrow::position.x) * (switchPos.x - arrow::position.x) +
				(switchPos.z - arrow::position.z) * (switchPos.z - arrow::position.z)
			);
			if (distance < switchRadius + arrow::radius && arrow::position.y > switchMinY && arrow::position.y < switchMaxY)
			{
				arrow::hasBeenShot = false;
				lampSwitchedOn[i] = !lampSwitchedOn[i];
			}
		}

		// dragon head collision
		float distance = sqrt(
			(headPosition.x - arrow::position.x) * (headPosition.x - arrow::position.x) +
			(headPosition.y - arrow::position.y) * (headPosition.y - arrow::position.y) +
			(headPosition.z - arrow::position.z) * (headPosition.z - arrow::position.z)
		);
		if (distance < arrow::radius)
		{
			arrow::hasBeenShot = false;
			dragon::hasBeenShot = true;
			dragon::isFalling = true;
			dragon::fallTime = 0.0f;
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
		drawObjectTexture(models::arrowContext,
			glm::translate(arrow::position) * arrow::cameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.5)),
			arrowTexture::albedo, arrowTexture::normal, arrowTexture::ambientOcclusion, arrowTexture::roughness, arrowTexture::metalness);
	}

	///// CROSSBOW

	// rotation matrix
	glm::vec3 crossbowSide = glm::normalize(glm::cross(crossbowDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 crossbowUp = glm::normalize(glm::cross(crossbowSide, crossbowDir));
	glm::mat4 crossbowCameraRotrationMatrix = glm::mat4({
		crossbowSide.x,crossbowSide.y,crossbowSide.z,0,
		crossbowUp.x,crossbowUp.y,crossbowUp.z ,0,
		-crossbowDir.x,-crossbowDir.y,-crossbowDir.z,0,
		0.,0.,0.,1.,
		});

	// color
	glm::vec3 crossbowColor = glm::vec3(0.1, 0.9, 0.1);
	if (arrow::hasBeenShot) crossbowColor = glm::vec3(0.9, 0.1, 0.1);
	if (dragon::hasBeenShot) crossbowColor = glm::vec3(0.9, 0.9, 0.1);

	// drawing
	//drawObjectPBR(models::crossbowContext,
	//	glm::translate(crossbowPos)* crossbowCameraRotrationMatrix* glm::eulerAngleY(glm::pi<float>())* glm::scale(glm::vec3(0.03f)),
	//	crossbowColor, 0.2, 1.0);
	drawObjectTexture(models::crossbowContext,
		glm::translate(crossbowPos) * crossbowCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.03f)),
		crossbowTexture::albedo, crossbowTexture::normal, crossbowTexture::ambientOcclusion, crossbowTexture::roughness, crossbowTexture::metalness);

	// update
	spotlightPos = crossbowPos + 0.2 * crossbowDir;
	spotlightConeDir = crossbowDir;
	for (int i = 0; i < torchPosition.size(); i++)
	{
		if (torchExisting[i])
		{
			particleUpdate(deltaTime, 50, glm::vec3(0.3f, 0.3f, 0.3f), firePosition[i]);
		}
	}
	//for (glm::vec3 firePos : firePosition) {
	//	particleUpdate(deltaTime, 50, glm::vec3(0.3f, 0.3f, 0.3f), firePos);
	//}


	//drawParticles();
	for (Particle particle : fire::particles) {

		drawObjectPBR(models::fireContext, glm::translate(particle.Position), particle.Color, 0.0f, 0.0f);
	}

	///// SCENE DRAWING

	// columns
	for (int i = 0; i < columnPosition.size(); i++)
	{
		if (columnDamaged[i])
		{
			if (!columnDestroyed[i])
			{
				drawObjectTexture(models::columnContext, glm::translate(columnPosition[i]),
					destroyedColumnTexture::albedo, destroyedColumnTexture::normal, destroyedColumnTexture::ambientOcclusion, destroyedColumnTexture::roughness, destroyedColumnTexture::metalness);
			}
		}
		else
		{
			drawObjectTexture(models::columnContext, glm::translate(columnPosition[i]),
				columnTexture::albedo, columnTexture::normal, columnTexture::ambientOcclusion, columnTexture::roughness, columnTexture::metalness);
		}
	}

	// torches
	for (int i = 0; i < torchPosition.size(); i++)
	{
		if (torchExisting[i])
		{
			drawTorch(glm::translate(torchPosition[i]), torchAngle[i]);
		}
	}

	// lamps
	for (int i = 0; i < lampPosition.size(); i++)
	{
		drawObjectTexture(models::lampContext, glm::translate(lampPosition[i]) * glm::eulerAngleY(glm::radians(lampAngle[i])) * glm::scale(glm::vec3(2.0f)),
			lampTexture::albedo, lampTexture::normal, lampTexture::ambientOcclusion, lampTexture::roughness, lampTexture::metalness);
	}

	// switches
	for (int i = 0; i < switchPosition.size(); i++)
	{
		drawObjectTexture(models::lightSwitchContext, glm::translate(switchPosition[i]) * glm::eulerAngleY(glm::radians(switchAngle[i])) * glm::scale(glm::vec3(2.0f)),
			lightSwitchTexture::albedo, lightSwitchTexture::normal, lightSwitchTexture::ambientOcclusion, lightSwitchTexture::roughness, lightSwitchTexture::metalness);
	}

	// other
	drawObjectTexture(models::doorLeftContext, glm::mat4(),
		doorTexture::albedo, doorTexture::normal, doorTexture::ambientOcclusion, doorTexture::roughness, doorTexture::metalness);
	drawObjectTexture(models::doorRightContext, glm::mat4(),
		doorTexture::albedo, doorTexture::normal, doorTexture::ambientOcclusion, doorTexture::roughness, doorTexture::metalness);
	drawObjectTexture(models::roomContext, glm::mat4(),
		roomTexture::albedo, roomTexture::normal, roomTexture::ambientOcclusion, roomTexture::roughness, roomTexture::metalness);

	// PATH EDITING
	if (dragon::isEditingPath)
	{
		glm::vec3 point = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
		if (dragon::currentEditedPath == 1)
		{
			point = dragon::path1[dragon::editedPointIndex];
			color = glm::vec3(1.0f, 0.0f, 0.0f);
		}
		else if (dragon::currentEditedPath == 2)
		{
			point = dragon::path2[dragon::editedPointIndex];
			color = glm::vec3(0.0f, 1.0f, 0.0f);
		}
		else if (dragon::currentEditedPath == 3)
		{
			point = dragon::path3[dragon::editedPointIndex];
			color = glm::vec3(0.0f, 0.0f, 1.0f);
		}
		drawObjectPBR(models::dragonSphere, glm::translate(point) * glm::scale(glm::vec3(0.5f)), color, 0.0f, 0.0f);
	}

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

	//skybox
	programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");
	loadModelToContext("./models/cube.obj", skyboxContext);
	char* cubemap[] = { "textures/skybox/posx.jpg", "textures/skybox/negx.jpg", "textures/skybox/posy.jpg", "textures/skybox/negy.jpg", "textures/skybox/posz.jpg", "textures/skybox/negz.jpg" };
	loadCubemap(cubemap);

	// shaders
	program = shaderLoader.CreateProgram("shaders/shader_9_1.vert", "shaders/shader_9_1.frag");
	programTest = shaderLoader.CreateProgram("shaders/test.vert", "shaders/test.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_8_sun.vert", "shaders/shader_8_sun.frag");
	programPBR = shaderLoader.CreateProgram("shaders/shader_PBR.vert", "shaders/shader_PBR.frag");

	// models
	loadModelToContext("./models/room.obj", models::roomContext);
	loadModelToContext("./models/column.obj", models::columnContext);
	loadModelToContext("./models/doorLeft.obj", models::doorLeftContext);
	loadModelToContext("./models/doorRight.obj", models::doorRightContext);
	loadModelToContext("./models/torch.obj", models::torchContext);
	loadModelToContext("./models/torchRings.obj", models::torchRingsContext);
	loadModelToContext("./models/torchHandle.obj", models::torchHandleContext);
	loadModelToContext("./models/lamp.obj", models::lampContext);
	loadModelToContext("./models/lightSwitch.obj", models::lightSwitchContext);
	loadModelToContext("./models/dragonHead.obj", models::dragonSphere);
	loadModelToContext("./models/dragonSegment.obj", models::dragonCylinder);
	loadModelToContext("./models/dragonTail.obj", models::dragonCone);
	loadModelToContext("./models/crossbow.obj", models::crossbowContext);
	loadModelToContext("./models/arrow.obj", models::arrowContext);
	loadModelToContext("./models/fire.obj", models::fireContext);

	// crossbow textures
	crossbowTexture::albedo = Core::LoadTexture("textures/crossbow/albedo.jpg");
	crossbowTexture::normal = Core::LoadTexture("textures/crossbow/normal.png");
	crossbowTexture::ambientOcclusion = Core::LoadTexture("textures/crossbow/ambientOcclusion.png");
	crossbowTexture::roughness = Core::LoadTexture("textures/crossbow/roughness.png");
	crossbowTexture::metalness = Core::LoadTexture("textures/crossbow/metalness.png");

	// destroyedColumn textures
	destroyedColumnTexture::albedo = Core::LoadTexture("textures/destroyedColumn/albedo.jpg");
	destroyedColumnTexture::normal = Core::LoadTexture("textures/destroyedColumn/normal.png");
	destroyedColumnTexture::ambientOcclusion = Core::LoadTexture("textures/destroyedColumn/ambientOcclusion.png");
	destroyedColumnTexture::roughness = Core::LoadTexture("textures/destroyedColumn/roughness.png");
	destroyedColumnTexture::metalness = Core::LoadTexture("textures/destroyedColumn/roughness.png");

	// column textures
	columnTexture::albedo = Core::LoadTexture("textures/column/albedo.jpg");
	columnTexture::normal = Core::LoadTexture("textures/column/normal.png");
	columnTexture::ambientOcclusion = Core::LoadTexture("textures/column/ambientOcclusion.png");
	columnTexture::roughness = Core::LoadTexture("textures/column/roughness.png");
	columnTexture::metalness = Core::LoadTexture("textures/column/roughness.png");

	// torch textures
	torchHandleTexture::albedo = Core::LoadTexture("textures/torchHandle/albedo.jpg");
	torchHandleTexture::normal = Core::LoadTexture("textures/torchHandle/normal.png");
	torchHandleTexture::ambientOcclusion = Core::LoadTexture("textures/torchHandle/ambientOcclusion.png");
	torchHandleTexture::roughness = Core::LoadTexture("textures/torchHandle/roughness.png");
	torchHandleTexture::metalness = Core::LoadTexture("textures/torchHandle/metalness.png");

	torchTexture::albedo = Core::LoadTexture("textures/torch/albedo.jpg");
	torchTexture::normal = Core::LoadTexture("textures/torch/normal.png");
	torchTexture::ambientOcclusion = Core::LoadTexture("textures/torch/ambientOcclusion.png");
	torchTexture::roughness = Core::LoadTexture("textures/torch/roughness.png");
	torchTexture::metalness = Core::LoadTexture("textures/torch/metalness.png");

	torchRingsTexture::albedo = Core::LoadTexture("textures/torchRings/albedo.jpg");
	torchRingsTexture::normal = Core::LoadTexture("textures/torchRings/normal.png");
	torchRingsTexture::ambientOcclusion = Core::LoadTexture("textures/torchRings/ambientOcclusion.png");
	torchRingsTexture::roughness = Core::LoadTexture("textures/torchRings/roughness.png");
	torchRingsTexture::metalness = Core::LoadTexture("textures/torchRings/metalness.png");

	//room textures darkened USE albedoOriginal.jpg TO GET BRIGHT TEXTURE
	roomTexture::albedo = Core::LoadTexture("textures/room/albedo.jpg");
	//roomTexture::albedo = Core::LoadTexture("textures/room/albedoOriginal.jpg");
	roomTexture::normal = Core::LoadTexture("textures/room/normal.png");
	roomTexture::ambientOcclusion = Core::LoadTexture("textures/room/ambientOcclusion.png");
	roomTexture::roughness = Core::LoadTexture("textures/room/roughness.png");
	roomTexture::metalness = Core::LoadTexture("textures/room/metalness.png");


	//there wasn't any ambientOcclusion for lamp
	lampTexture::albedo = Core::LoadTexture("textures/lamp/albedo.jpg");
	lampTexture::normal = Core::LoadTexture("textures/lamp/normal.png");
	lampTexture::ambientOcclusion = Core::LoadTexture("textures/lamp/ambientOcclusion.png");
	lampTexture::roughness = Core::LoadTexture("textures/lamp/metalness.png");
	lampTexture::metalness = Core::LoadTexture("textures/lamp/metalness.png");

	//light switch textures
	lightSwitchTexture::albedo = Core::LoadTexture("textures/lightSwitch/albedo.jpg");
	lightSwitchTexture::normal = Core::LoadTexture("textures/lightSwitch/normal.png");
	lightSwitchTexture::ambientOcclusion = Core::LoadTexture("textures/lightSwitch/ambientOcclusion.png");
	lightSwitchTexture::roughness = Core::LoadTexture("textures/lightSwitch/roughness.png");
	lightSwitchTexture::metalness = Core::LoadTexture("textures/lightSwitch/metalness.png");


	//there wasn't any ambientOcclusion for arrow
	arrowTexture::albedo = Core::LoadTexture("textures/arrow/albedo.jpg");
	arrowTexture::normal = Core::LoadTexture("textures/arrow/normal.png");
	arrowTexture::ambientOcclusion = Core::LoadTexture("textures/arrow/ambientOcclusion.png");
	arrowTexture::roughness = Core::LoadTexture("textures/arrow/roughness.png");
	arrowTexture::metalness = Core::LoadTexture("textures/arrow/metalness.png");

	//door textures
	doorTexture::albedo = Core::LoadTexture("textures/door/albedo.jpg");
	doorTexture::normal = Core::LoadTexture("textures/door/normal.png");
	doorTexture::ambientOcclusion = Core::LoadTexture("textures/door/ambientOcclusion.png");
	doorTexture::roughness = Core::LoadTexture("textures/door/roughness.png");
	doorTexture::metalness = Core::LoadTexture("textures/door/metalness.png");

	// path
	loadPathsFromFile();
	loadDeltaTimeFromFile();
	dragon::currentPath = int(deltaTime * 100000) % 3 + 1;
	if (dragon::currentPath == 1)
		dragon::spline.points = dragon::path1;
	else if (dragon::currentPath == 2)
		dragon::spline.points = dragon::path2;
	else if (dragon::currentPath == 3)
		dragon::spline.points = dragon::path3;
	initPath();
	initParticles();
}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);

	savePathsToFile();
	saveDeltaTimeToFile();
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	glm::vec3 crossbowSide = glm::normalize(glm::cross(crossbowDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 crossbowUp = glm::vec3(0.f, 1.f, 0.f);
	float angleSpeed = 0.05f * deltaTime * 60;
	float moveSpeed = 0.05f * deltaTime * 60;

	if (dragon::isEditingPath)
	{
		float change = 0.5f;
		glm::vec3 point = glm::vec3(0.0f);
		if (dragon::currentEditedPath == 1)
			point = dragon::path1[dragon::editedPointIndex];
		else if (dragon::currentEditedPath == 2)
			point = dragon::path2[dragon::editedPointIndex];
		else if (dragon::currentEditedPath == 3)
			point = dragon::path3[dragon::editedPointIndex];

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			point.z += change;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			point.z -= change;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			point.y += change;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			point.y -= change;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			point.x += change;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			point.x -= change;

		if (dragon::currentEditedPath == 1)
			dragon::path1[dragon::editedPointIndex] = point;
		else if (dragon::currentEditedPath == 2)
			dragon::path2[dragon::editedPointIndex] = point;
		else if (dragon::currentEditedPath == 3)
			dragon::path3[dragon::editedPointIndex] = point;
	}
	else
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			crossbowPos += crossbowDir * moveSpeed;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			crossbowPos -= crossbowDir * moveSpeed;
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
			crossbowPos += crossbowSide * moveSpeed;
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
			crossbowPos -= crossbowSide * moveSpeed;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			crossbowPos += crossbowUp * moveSpeed;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			crossbowPos -= crossbowUp * moveSpeed;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			crossbowDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(crossbowDir, 0));
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			crossbowDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(crossbowDir, 0));
	}

	cameraPos = crossbowPos - 0.5 * crossbowDir + glm::vec3(0, 1, 0) * 0.2f;
	cameraDir = crossbowDir;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		exposition -= 0.05;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		exposition += 0.05;

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		printf("crossbowPos = glm::vec3(%ff, %ff, %ff);\n", crossbowPos.x, crossbowPos.y, crossbowPos.z);
		printf("crossbowDir = glm::vec3(%ff, %ff, %ff);\n", crossbowDir.x, crossbowDir.y, crossbowDir.z);
		printf("dragon::isEditingPath = %s);\n", dragon::isEditingPath ? "true" : "false");
		printf("dragon::currentEditedPath = %i);\n", dragon::currentEditedPath);
		printf("dragon::editedPointIndex = %i);\n", dragon::editedPointIndex);
	}

	//cameraDir = glm::normalize(-cameraPos);

	///// ARROW

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		//if (!arrow::hasBeenShot)
		if(!shot)
		{
			arrow::hasBeenShot = true;
			arrow::time = 0.0f;
			arrow::position = crossbowPos;
			arrow::direction = crossbowDir;
			shot = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		shot = false;
	}

	// PATH EDITING
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		if (dragon::isEditingPath)
		{
			dragon::currentPath = dragon::currentEditedPath;
			if (dragon::currentPath == 1)
				dragon::spline.points = dragon::path1;
			else if (dragon::currentPath == 2)
				dragon::spline.points = dragon::path2;
			else if (dragon::currentPath == 3)
				dragon::spline.points = dragon::path3;
			initPath();
			savePathsToFile();
			dragon::isEditingPath = false;
		}
		else
		{
			dragon::currentEditedPath = dragon::currentPath;
			dragon::isEditingPath = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		if (dragon::isEditingPath)
		{
			if (dragon::currentEditedPath == 1)
				dragon::currentEditedPath = 2;
			else if (dragon::currentEditedPath == 2)
				dragon::currentEditedPath = 3;
			else if (dragon::currentEditedPath == 3)
				dragon::currentEditedPath = 1;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		if (dragon::isEditingPath)
		{
			dragon::editedPointIndex--;
			if (dragon::editedPointIndex == -1)
				dragon::editedPointIndex = 49;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		if (dragon::isEditingPath)
		{
			dragon::editedPointIndex++;
			if (dragon::editedPointIndex == 50)
				dragon::editedPointIndex = 0;
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

void loadPathsFromFile()
{
	std::ifstream path1("src/path1.txt");
	std::ifstream path2("src/path2.txt");
	std::ifstream path3("src/path3.txt");

	std::string text;

	for (int i = 0; i < 50; i++)
	{
		std::getline(path1, text);
		float x1 = std::stof(text);
		std::getline(path1, text);
		float y1 = std::stof(text);
		std::getline(path1, text);
		float z1 = std::stof(text);

		std::getline(path2, text);
		float x2 = std::stof(text);
		std::getline(path2, text);
		float y2 = std::stof(text);
		std::getline(path2, text);
		float z2 = std::stof(text);

		std::getline(path3, text);
		float x3 = std::stof(text);
		std::getline(path3, text);
		float y3 = std::stof(text);
		std::getline(path3, text);
		float z3 = std::stof(text);

		dragon::path1[i] = glm::vec3(x1, y1, z1);
		dragon::path2[i] = glm::vec3(x2, y2, z2);
		dragon::path3[i] = glm::vec3(x3, y3, z3);
	}

	path1.close();
	path2.close();
	path3.close();
}

void savePathsToFile()
{
	std::ofstream path1("src/path1.txt");
	std::ofstream path2("src/path2.txt");
	std::ofstream path3("src/path3.txt");

	for (int i = 0; i < 50; i++)
	{
		std::string x1 = std::to_string(dragon::path1[i].x);
		std::string y1 = std::to_string(dragon::path1[i].y);
		std::string z1 = std::to_string(dragon::path1[i].z);

		std::string x2 = std::to_string(dragon::path2[i].x);
		std::string y2 = std::to_string(dragon::path2[i].y);
		std::string z2 = std::to_string(dragon::path2[i].z);

		std::string x3 = std::to_string(dragon::path3[i].x);
		std::string y3 = std::to_string(dragon::path3[i].y);
		std::string z3 = std::to_string(dragon::path3[i].z);

		path1 << x1 << std::endl << y1 << std::endl << z1 << std::endl;
		path2 << x2 << std::endl << y2 << std::endl << z2 << std::endl;
		path3 << x3 << std::endl << y3 << std::endl << z3 << std::endl;
	}

	path1.close();
	path2.close();
	path3.close();
}


void loadDeltaTimeFromFile()
{
	std::ifstream file("src/deltaTime.txt");
	std::string text;
	std::getline(file, text);
	deltaTime = std::stof(text);
	file.close();
}

void saveDeltaTimeToFile()
{
	std::ofstream file("src/deltaTime.txt");
	std::string text = std::to_string(deltaTime);
	file << text;
	file.close();
}