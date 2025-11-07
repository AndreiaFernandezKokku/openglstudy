#include "Object3D.h"

void Object3D::CreateBuffer(std::vector<Vertex> ver, std::vector<unsigned int> indices)
{
	renderBuffer.elementCount = indices.size();
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &renderBuffer.vbo);

	glGenBuffers(1, &renderBuffer.ibo);

	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, renderBuffer.vbo);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * ver.size(), &ver[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderBuffer.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

Object3D::Object3D(std::vector<Vertex> ver, std::vector<unsigned int> indices)
{
	CreateBuffer(ver, indices);
	transform = glm::mat4(1.f);
}

Object3D::~Object3D()
{
	glDeleteBuffers(1, &renderBuffer.vbo);
	glDeleteBuffers(1, &renderBuffer.ibo);
}

void Object3D::SetPos(const glm::vec3& pos)
{
	transform = glm::translate(glm::mat4(1.f), pos);
}

void Object3D::Rotate(const glm::vec3& rot, float angle)
{
	transform = glm::rotate(transform, angle, rot);
}

void Object3D::SetRot(const glm::vec3& rot, float angle)
{
	transform = glm::rotate(glm::mat4(1.f), angle, rot);
}

void Object3D::SetScale(const glm::vec3& scale)
{
	transform = glm::scale(glm::mat4(1.f), scale);
}
