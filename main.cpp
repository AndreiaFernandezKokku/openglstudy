#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "ImageCustomLoader.h"
#include <glm/gtc/type_ptr.hpp>
#include "common/Controls.h"
#include "Object3D.h"
#include "Renderer.h"

int windowWidth = 1024;
int windowHeight = 768;

void error_callback(int error_code, const char* description) {
	fprintf(stderr, "GLFW Error %d: %s\n", error_code, description);
}

bool InitializeGLFW() 
{
	glewExperimental = true;
	glfwSetErrorCallback(error_callback);
	return glfwInit();
}

GLFWwindow* OpenGLWindow()
{
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	if (!InitializeGLFW()) return nullptr;

	GLFWwindow* glfwWindow = nullptr;
	glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "Tela", nullptr, nullptr);
	if (glfwWindow == nullptr) 
	{
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(glfwWindow);
	return glfwWindow;
}

//cube vertexes
static const std::vector<Vertex> cube_vertex_buffer_data =
{
	Vertex(1.0f, 1.0f,-1.0f, 1.f, 0.f, 0.f, 1.0f, 1.0f), //0
	Vertex(-1.0f, 1.0f, -1.0f, 0.583f, 0.771f, 0.014f, 0.0f, 1.0f), //1
	Vertex(1.0f, 1.0f, 1.0f, 0.609f, 0.115f, 0.436f, 1.0f, 1.0f), //2
	Vertex(-1.0f, 1.0f, 1.0f, 0.327f, 0.483f, 0.844f, 0.0f, 1.0f), //3
	Vertex(1.0f, -1.0f, -1.0f, 0.435f, 0.602f, 0.223f, 1.0f, 0.0f), //4
	Vertex(-1.0f,-1.0f, -1.0f, 0.559f, 0.436f, 0.730f, 0.0f, 0.0f), //5
	Vertex(-1.0f, -1.0f, 1.0f, 0.359f, 0.583f, 0.152f, 0.0f, 0.0f), //6
	Vertex(1.0f, -1.0f, 1.0f, 0.517f, 0.713f, 0.338f, 1.0f, 0.0f) //7
};

static const std::vector<unsigned int> cube_indices =
{
	0,1,4,5,6,1,3,0,2,4,7,6,2,3
};

static const std::vector<Vertex> plane_vertex_buffer_data =
{
	Vertex(1,0,1,1,1,1,0,0),
	Vertex(1,0,-1,1,1,1,0,1),
	Vertex(-1,0,-1,1,1,1,1,1),
	Vertex(-1,0,1,1,1,1,1,0)
};

static const std::vector<unsigned int> plane_indices =
{
	0,1,3,2
};


glm::mat4 ProjectionView() 
{
	// Projection matrix: 45� Field of View, 4:3 ratio, display range: 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 4, 4), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Our ModelViewProjection: multiplication of our 3 matrices
	glm::mat4 mvp = Projection * View; // Remember, matrix multiplication is the other way 

	return mvp;
}

glm::mat4 Model(float angle)
{
	// Model matrix: an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	Model = glm::rotate(Model, glm::radians(angle), glm::vec3(0, 1, 0));
	return Model;
}

void DrawRenderTarget(GLuint target)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, 0);

}

void DrawScene(const glm::mat4& vp, Renderer& renderer, const std::vector<Object3D*>& meshes, GLuint shader)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (Object3D* mesh : meshes)
	{
		renderer.DrawMesh(mesh, shader, vp);
	}
}

int Render(GLFWwindow* window)
{
	//ImageCustomLoader loader = ImageCustomLoader();
	//GLuint Texture = loader.LoadBMP_custom("imagemodel.bmp");
	Renderer renderer;
	renderer.CreateTexture("imagemodel.bmp");


	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	//instead of back face culling, we do front face culling due to the nature of the instantiated cube
	glCullFace(GL_FRONT);

	Object3D mesh(cube_vertex_buffer_data, cube_indices);
	Object3D plane(plane_vertex_buffer_data, plane_indices);
	plane.SetScale(glm::vec3(2.f, 2.f, 2.f));
	plane.Rotate(glm::vec3(1.f, 0.f, 0.f), glm::radians(180.f));
	plane.Translate(glm::vec3(0.f, 1.5f, 0.f));
	if (const Texture* t = renderer.GetTexture("imagemodel.bmp"))
	{
		mesh.SetTexture(t->id);
	}
	
	RenderTexture renderTarget = renderer.CreateRenderTarget("renderTarget", windowWidth, windowHeight);
	plane.SetTexture(renderTarget.id);

	GLuint shader = renderer.CreateShader("vs.vert", "fs.frag");

	float angle = 10.f;
	glm::mat4 vp = ProjectionView();
	double lastTime = glfwGetTime();
	std::vector<Object3D*> meshes = { &mesh, &plane };

	do 
	{
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);
		lastTime = currentTime;

		mesh.Rotate(glm::vec3(0.f, 0.f, 1.f), glm::radians(angle * deltaTime));
		plane.parentTransform = mesh.GetTransform();

		vp = ComputeMatricesFromInputs(window, deltaTime);
		glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.frameBufferId);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTarget.id, 0);
		GLenum draws[1] = { GL_COLOR_ATTACHMENT0 };

		glViewport(0, 0, windowWidth, windowHeight);
		DrawScene(vp, renderer, { &mesh }, shader);
		glDrawBuffers(1, draws);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		DrawScene(vp, renderer, meshes, shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
	renderer.Dispose();
	return 0;
}

int main() 
{
	GLFWwindow* window = OpenGLWindow();
	if (window == nullptr) return -2;
	glewInit();
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	return Render(window);
}