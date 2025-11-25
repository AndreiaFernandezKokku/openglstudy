#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <map>
#include "common/Sphere.h"
#include <memory>

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 uvCord;

	Vertex(float x, float y, float z, float r = 1.f, float g = 1.f, float b = 1.f, float u = 1.f, float v = 1.f)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;
		color.r = r;
		color.g = g;
		color.b = b;
		uvCord.x = u;
		uvCord.y = v;
	}
};

struct RenderBuffer
{
	GLuint vbo; //vertex buffer object
	GLuint ibo; //index buffer object
	unsigned int elementCount;
};

struct Shader
{
	GLuint id;

	Shader(GLuint _id)
	{
		id = _id;
	}
	Shader() = default;
};

struct Texture
{
	GLuint texId = 0;
	unsigned int Width = 0;
	unsigned int Height = 0;

	Texture(GLuint _id)
	{
		texId = _id;	
	}
	Texture() = default;
};

struct RenderTexture : public Texture
{

	GLuint frameBufferId = 0;

	RenderTexture(GLuint _id) : Texture(_id) { frameBufferId = 0; }
	RenderTexture(GLuint _id, GLuint _buffer) :  Texture(_id)
	{
		frameBufferId = _buffer;
	}
	RenderTexture() : Texture()
	{
		frameBufferId = 0;
	}
};

class Object3D; //fwd declaration

static Sphere lightSphere(0.0125f);

class BaseLight 
{
public:
	glm::vec3 Position;
	glm::vec3 Color;
};

class PointLight : public BaseLight
{
public:
	float constant = 1.0f;
	float linear = 0.22f;
    float quadratic = 0.20f;
};

class AmbientLight : public BaseLight
{
};

class DirectionalLight : public BaseLight
{
public:
	float Intensity;
	glm::vec3 Rotation;
};

class Renderer
{
protected:
	std::map<std::string, Shader> ShaderPool;
	std::map<std::string, Texture> TexturePool;
	std::map<std::string, RenderTexture> RenderTexturePool;
	AmbientLight _AmbientLight;
	DirectionalLight _DirectionalLight;
	std::vector<PointLight> _PointLights;
	GLuint PointLightsUniformBufferObjectId;

public:
	void Initialize();
	void UploadLightData();
	void DrawMesh(const Object3D* mesh, GLuint shader, const glm::mat4& vp, bool useLights = true);
	void PrepareLights(GLuint shader);
	GLuint CreateShader(std::string vertex_file_path, std::string fragment_file_path, std::string shaderName = "");
	const Texture& CreateTexture(std::string texturePath, std::string name = "");
	const RenderTexture& CreateRenderTarget(std::string name, GLuint width, GLuint height, GLuint glformat = GL_RGB);
	const Texture* GetTexture(std::string name);
	const RenderTexture* GetRenderTexture(std::string name);
	void AddPointLight(glm::vec3 position, glm::vec3 color, float intensity);
	void SetAmbientLight(glm::vec3 color);
	void SetDirectionalLight(glm::vec3 color, float intensity, glm::vec3 rotation);
	void Dispose();

	//Debugging
	void DrawDebugLights(const glm::mat4 &vp);
};