#pragma once
#include "Renderer.h"

class Object3D
{
protected:
	glm::mat4 transform = glm::mat4(1.f);
	RenderBuffer renderBuffer;
	Material material;

public:
	glm::mat4 parentTransform = glm::mat4(1.f);

	Object3D(std::vector<Vertex> ver, std::vector<unsigned int> indices);
	Object3D() = default;
	~Object3D();
	void CreateBuffer(std::vector<Vertex> ver, std::vector<unsigned int> indices);
	void SetPos(const glm::vec3& pos);
	void Translate(const glm::vec3& pos); //related to the object's axis
	void Rotate(const glm::vec3& rot, float angle);
	void SetRot(const glm::vec3& rot, float angle);
	void SetScale(const glm::vec3& scale);
	const RenderBuffer& GetRenderBuffer() const { return renderBuffer; }
	const glm::mat4& GetTransform() const { return parentTransform * transform; }
	unsigned int IndexSize() const { return renderBuffer.elementCount; }
	void SetTexture(const Texture* _texture);
	void SetShader(const Shader* _shader);
	const GLuint GetTexture() const { return material.texture == nullptr ? (GLuint)0 : material.texture->texId; }
	const GLuint GetShader() const { return material.shader == nullptr ? (GLuint)0 : material.shader->id; }
	bool IsInitialized() const { return renderBuffer.elementCount < 0;  }

};

