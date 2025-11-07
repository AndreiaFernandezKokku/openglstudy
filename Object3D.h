#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 uvCord;

	Vertex(float x, float y, float z, float r, float g, float b, float u, float v)
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

class Object3D
{
protected:
	glm::mat4 transform;
	RenderBuffer renderBuffer;
	void CreateBuffer(std::vector<Vertex> ver, std::vector<unsigned int> indices);

public:
	Object3D(std::vector<Vertex> ver, std::vector<unsigned int> indices);
	~Object3D();
	void SetPos(const glm::vec3& pos);
	void Rotate(const glm::vec3& rot, float angle);
	void SetRot(const glm::vec3& rot, float angle);
	void SetScale(const glm::vec3& scale);
	RenderBuffer& GetRenderBuffer() { return renderBuffer; }
	glm::mat4& GetTransform() { return transform; }
	unsigned int IndexSize() { return renderBuffer.elementCount; }
};

