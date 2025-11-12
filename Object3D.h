#pragma once
#include "Renderer.h"

class Object3D
{
protected:
	glm::mat4 transform;
	RenderBuffer renderBuffer;
	GLuint texture;
	void CreateBuffer(std::vector<Vertex> ver, std::vector<unsigned int> indices);

public:
	glm::mat4 parentTransform;

	Object3D(std::vector<Vertex> ver, std::vector<unsigned int> indices);
	~Object3D();
	void SetPos(const glm::vec3& pos);
	void Translate(const glm::vec3& pos); //related to the object's axis
	void Rotate(const glm::vec3& rot, float angle);
	void SetRot(const glm::vec3& rot, float angle);
	void SetScale(const glm::vec3& scale);
	const RenderBuffer& GetRenderBuffer() const { return renderBuffer; }
	const glm::mat4& GetTransform() const { return parentTransform * transform; }
	unsigned int IndexSize() const { return renderBuffer.elementCount; }
	void SetTexture(GLuint textureID);
	const GLuint GetTexture() const { return texture; }

};

