#include "Renderer.h"
#include "Object3D.h"
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "ImageCustomLoader.h"

const unsigned int MAX_LIGHTS = 32;
Object3D debugLightSphere;

void Renderer::Initialize()
{
	glGenBuffers(1, &PointLightsUniformBufferObjectId);
	glBindBuffer(GL_UNIFORM_BUFFER, PointLightsUniformBufferObjectId);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLight) * MAX_LIGHTS, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	if (!debugLightSphere.IsInitialized())
	{
		const Texture& t = CreateTexture("notex.bmp", "default");
		const Shader& sh = CreateShader("vs.vert", "unlit.frag", "unlit");
		Sphere s(1.f);
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		vertices.reserve(s.getVertexCount());
		indices.reserve(s.getIndexCount());
		const float* verts = s.getVertices();
		const unsigned int* idx = s.getIndices();
		for (int i = 0; i < s.getVertexSize() * 3; i += 3)
		{
			Vertex v(verts[i], verts[i + 1], verts[i + 2]);
			vertices.emplace_back(v);
		}
		for (int j = 0; j < s.getIndexCount(); ++j)
		{
			indices.push_back(idx[j]);
		}
		debugLightSphere.CreateBuffer(vertices, indices);
		debugLightSphere.SetTexture(t.texId);
	}

}

void Renderer::UploadLightData()
{
	glBindBuffer(GL_UNIFORM_BUFFER, PointLightsUniformBufferObjectId);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, PointLightsUniformBufferObjectId);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLight)* _PointLights.size(), &_PointLights[0]);
}

void Renderer::DrawMesh(const Object3D* mesh, GLuint shader, const glm::mat4& vp, bool useLights)
{
	const RenderBuffer renderBuffer = mesh->GetRenderBuffer();
	glEnableVertexAttribArray(0);//vertex
	glBindBuffer(GL_ARRAY_BUFFER, renderBuffer.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderBuffer.ibo);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized
		8 * sizeof(GLfloat),                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(1); //color
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(GLfloat),
		(void*)(3 * sizeof(GLfloat))
	);
	glEnableVertexAttribArray(2); //uv
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(GLfloat),
		(void*)(6 * sizeof(GLfloat))
	);
	glUseProgram(shader);

	GLuint MatrixID = glGetUniformLocation(shader, "MVP");
	glm::mat4 mvp = vp * mesh->GetTransform();
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(mvp));
	if (useLights)
	{
		PrepareLights(shader);

		GLuint pointLightsId = glGetUniformBlockIndex(shader, "myPointLights");
		glUniformBlockBinding(shader, pointLightsId, 0);
		glBindBuffer(GL_UNIFORM_BUFFER, PointLightsUniformBufferObjectId);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, PointLightsUniformBufferObjectId);
	}

	glBindTexture(GL_TEXTURE_2D, mesh->GetTexture());
	glDrawElements(GL_TRIANGLE_STRIP, mesh->IndexSize(), GL_UNSIGNED_INT, (void*)0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::PrepareLights(GLuint shader)
{
	GLuint LightID = glGetUniformLocation(shader, "myAmbientLight.Color");
	glUniform3fv(LightID, 1, glm::value_ptr(_AmbientLight.Color));
	GLuint numLightsID = glGetUniformLocation(shader, "numLights");
	glUniform1i(numLightsID, _PointLights.size());
}

//fragment shader == pixel shader
GLuint Renderer::CreateShader(std::string vertex_file_path, std::string fragment_file_path, std::string shaderName)
{
	std::string hash = shaderName;
	if (hash.size() == 0)
	{
		hash = vertex_file_path;
		hash.append("|");
		hash.append(fragment_file_path);
	}
	if (ShaderPool.count(hash)) 
	{
		return ShaderPool[hash].id;
	}
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	ShaderPool[hash] = Shader(ProgramID);

	return ProgramID;
}

const Texture& Renderer::CreateTexture(std::string texturePath, std::string name)
{
	if (name.size() == 0)
	{
		name = texturePath;
	}
	if (TexturePool.count(name))
	{
		return TexturePool[name];
	}
	ImageCustomLoader loader = ImageCustomLoader();
	GLuint texture = loader.LoadBMP_custom("imagemodel.bmp");
	glBindTexture(GL_TEXTURE_2D, 0);
	TexturePool[name] = Texture(texture);
	return TexturePool[name];
}

const RenderTexture& Renderer::CreateRenderTarget(std::string name, GLuint width, GLuint height, GLuint glformat)
{
	if (RenderTexturePool.count(name))
	{
		return RenderTexturePool[name];
	}

	GLuint renderTarget; 
	GLuint renderBuffer;
	glGenFramebuffers(1, &renderBuffer);
	glGenTextures(1, &renderTarget);
	glBindTexture(GL_TEXTURE_2D, renderTarget);

	glTexImage2D(GL_TEXTURE_2D, 0, glformat, width, height, 0, glformat, GL_UNSIGNED_BYTE, 0);
	//here we do not want mip map since this is a render target
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	RenderTexturePool[name] = RenderTexture(renderTarget, renderBuffer);
	return RenderTexturePool[name];
}

const Texture *Renderer::GetTexture(std::string name) 
{
	return TexturePool.count(name) ? &TexturePool[name] : nullptr;
}

const RenderTexture *Renderer::GetRenderTexture(std::string name) 
{
	return RenderTexturePool.count(name) ? &RenderTexturePool[name] : nullptr;
}

void Renderer::AddPointLight(glm::vec3 position, glm::vec3 color, float intensity)
{
	PointLight _light;
	_light.Color = color;
	_light.Position = position;

	_PointLights.emplace_back(_light);
}

void Renderer::SetAmbientLight(glm::vec3 color)
{
	_AmbientLight.Color = color;
}

void Renderer::SetDirectionalLight(glm::vec3 color, float intensity, glm::vec3 rotation)
{
	_DirectionalLight.Color = color;
	_DirectionalLight.Intensity = intensity;
	_DirectionalLight.Rotation = rotation;
}

void Renderer::Dispose()
{
	for (auto s : ShaderPool)
	{
		glDeleteProgram(s.second.id);
	}
	ShaderPool.clear();

	for (auto t : TexturePool)
	{
		glDeleteTextures(1, &t.second.texId);
	}
	TexturePool.clear();

	for (auto rt : RenderTexturePool)
	{
		glDeleteTextures(1, &rt.second.texId);
		glDeleteBuffers(1, &rt.second.frameBufferId);
	}
	RenderTexturePool.clear();

}

void Renderer::DrawDebugLights(const glm::mat4 &vp)
{
	
	for (const PointLight& l : _PointLights)
	{
		debugLightSphere.SetPos(l.Position);
		DrawMesh(&debugLightSphere, ShaderPool["unlit"].id, vp, false);
	}
}
