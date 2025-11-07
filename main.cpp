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
	0,1,2,3
};

//fragment shader == pixel shader
GLuint CreateShader(std::string vertex_file_path, std::string fragment_file_path)
{
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

	return ProgramID;
}

void DrawMesh(Object3D mesh, GLuint shader, const glm::mat4& vp)
{
	const RenderBuffer renderBuffer = mesh.GetRenderBuffer();
	glEnableVertexAttribArray(0);//vertex
	glBindBuffer(GL_ARRAY_BUFFER, renderBuffer.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderBuffer.ibo);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized
		8*sizeof(GLfloat),                  // stride
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
	glm::mat4 mvp = vp * mesh.GetTransform();
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(mvp));
	glDrawElements(GL_TRIANGLE_STRIP, mesh.IndexSize(), GL_UNSIGNED_INT, (void*)0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

glm::mat4 ProjectionView() 
{
	// Projection matrix: 45° Field of View, 4:3 ratio, display range: 0.1 unit <-> 100 units
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

int Render(GLFWwindow* window)
{
	ImageCustomLoader loader = ImageCustomLoader();
	GLuint Texture = loader.LoadBMP_custom("imagemodel.bmp");
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

	GLuint shader = CreateShader("vs.vert", "fs.frag");

	float angle = 10.f;
	glm::mat4 vp = ProjectionView();
	double lastTime = glfwGetTime();

	do 
	{
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);
		lastTime = currentTime;

		vp = ComputeMatricesFromInputs(window, deltaTime);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mesh.Rotate(glm::vec3(0.f, 1.f, 0.f), glm::radians(angle * deltaTime));

		DrawMesh(mesh, shader, vp);
		DrawMesh(plane, shader, vp);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDeleteProgram(shader);
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