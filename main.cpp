#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "ImageCustomLoader.h"

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

//cube
// Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
static const Vertex g_vertex_buffer_data[] = {
	 Vertex(-1.0f,-1.0f,-1.0f, 1.f, 0.f, 0.f, 0.000059f, 1.0f - 0.000004f),
	 Vertex(-1.0f,-1.0f, 1.0f, 0.583f, 0.771f, 0.014f, 0.000103f, 1.0f - 0.336048f),
	 Vertex(-1.0f, 1.0f, 1.0f, 0.609f, 0.115f, 0.436f, 0.335973f, 1.0f - 0.335903f),
	 Vertex(1.0f, 1.0f,-1.0f, 0.327f, 0.483f, 0.844f, 1.000023f, 1.0f - 0.000013f),
	 Vertex(-1.0f,-1.0f,-1.0f, 0.822f, 0.569f, 0.201f, 0.667979f, 1.0f - 0.335851f),
	 Vertex(-1.0f, 1.0f,-1.0f, 0.435f, 0.602f, 0.223f, 0.999958f, 1.0f - 0.336064f),
	 Vertex(1.0f,-1.0f, 1.0f, 0.310f, 0.747f, 0.185f, 0.667979f, 1.0f - 0.335851f),
	 Vertex(-1.0f,-1.0f,-1.0f, 0.597f, 0.770f, 0.761f, 0.336024f, 1.0f - 0.671877f),
	 Vertex(1.0f,-1.0f,-1.0f, 0.559f, 0.436f, 0.730f, 0.667969f, 1.0f - 0.671889f),
	 Vertex(1.0f, 1.0f,-1.0f, 0.359f, 0.583f, 0.152f, 1.000023f, 1.0f - 0.000013f),
	 Vertex(1.0f,-1.0f,-1.0f,0.483f, 0.596f, 0.789f, 0.668104f, 1.0f - 0.000013f),
	 Vertex(-1.0f,-1.0f,-1.0f, 0.559f, 0.861f, 0.639f, 0.667979f, 1.0f - 0.335851f),
	 Vertex(-1.0f,-1.0f,-1.0f, 0.195f, 0.548f, 0.859f, 0.000059f, 1.0f - 0.000004f),
	 Vertex(-1.0f, 1.0f, 1.0f,0.014f, 0.184f, 0.576f, 0.335973f, 1.0f - 0.335903f),
	 Vertex(-1.0f, 1.0f,-1.0f, 0.771f, 0.328f, 0.970f, 0.336098f, 1.0f - 0.000071f),
	 Vertex(1.0f,-1.0f, 1.0f, 0.406f, 0.615f, 0.116f, 0.667979f, 1.0f - 0.335851f),
	 Vertex(-1.0f,-1.0f, 1.0f, 0.676f, 0.977f, 0.133f, 0.335973f, 1.0f - 0.335903f),
	 Vertex(-1.0f,-1.0f,-1.0f, 0.971f, 0.572f, 0.833f, 0.336024f, 1.0f - 0.671877f),
	 Vertex(-1.0f, 1.0f, 1.0f, 0.140f, 0.616f, 0.489f, 1.000004f, 1.0f - 0.671847f),
	 Vertex(-1.0f,-1.0f, 1.0f, 0.997f, 0.513f, 0.064f, 0.999958f, 1.0f - 0.336064f),
	 Vertex(1.0f,-1.0f, 1.0f, 0.945f, 0.719f, 0.592f, 0.667979f, 1.0f - 0.335851f),
	 Vertex(1.0f, 1.0f, 1.0f, 0.543f, 0.021f, 0.978f, 0.668104f, 1.0f - 0.000013f),
	 Vertex(1.0f,-1.0f,-1.0f, 0.279f, 0.317f, 0.505f, 0.335973f, 1.0f - 0.335903f),
	 Vertex(1.0f, 1.0f,-1.0f, 0.167f, 0.620f, 0.077f, 0.667979f, 1.0f - 0.335851f),
	 Vertex(1.0f,-1.0f,-1.0f, 0.347f, 0.857f, 0.137f, 0.335973f, 1.0f - 0.335903f),
	 Vertex(1.0f, 1.0f, 1.0f, 0.055f, 0.953f, 0.042f, 0.668104f, 1.0f - 0.000013f),
	 Vertex(1.0f,-1.0f, 1.0f, 0.714f, 0.505f, 0.345f, 0.336098f, 1.0f - 0.000071f),
	 Vertex(1.0f, 1.0f, 1.0f, 0.783f, 0.290f, 0.734f, 0.000103f, 1.0f - 0.336048f),
	 Vertex(1.0f, 1.0f,-1.0f, 0.722f, 0.645f, 0.174f, 0.000004f, 1.0f - 0.671870f),
	 Vertex(-1.0f, 1.0f,-1.0f, 0.302f, 0.455f, 0.848f, 0.336024f, 1.0f - 0.671877f),
	 Vertex(1.0f, 1.0f, 1.0f, 0.517f, 0.713f, 0.338f, 0.000103f, 1.0f - 0.336048f),
	 Vertex(-1.0f, 1.0f,-1.0f, 0.053f, 0.959f, 0.120f, 0.336024f, 1.0f - 0.671877f),
	 Vertex(-1.0f, 1.0f, 1.0f, 0.393f, 0.621f, 0.362f, 0.335973f, 1.0f - 0.335903f),
	 Vertex(1.0f, 1.0f, 1.0f, 0.673f, 0.211f, 0.457f, 0.667969f, 1.0f - 0.671889f),
	 Vertex(-1.0f, 1.0f, 1.0f,  0.820f, 0.883f, 0.371f, 1.000004f, 1.0f - 0.671847f),
	 Vertex(1.0f,-1.0f, 1.0f, 0.982f, 0.099f, 0.879f, 0.667979f, 1.0f - 0.335851f)
};

GLuint CreateBuffer()
{
	// This will identify our vertex buffer
	GLuint vertexbuffer = 0;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	return vertexbuffer;
}

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

void DrawMesh(GLuint vertexBuffer, GLuint shader)
{
	glEnableVertexAttribArray(0);//vertex
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
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

	//Draw the cube!
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles -> 6 squares
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

glm::mat4 ProjectionViewModel() 
{
	// Projection matrix: 45° Field of View, 4:3 ratio, display range: 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 4, 4), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix: an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection: multiplication of our 3 matrices
	glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way 

	return mvp;
}

int Render(GLFWwindow* window)
{
	ImageCustomLoader loader = ImageCustomLoader();
	GLuint Texture = loader.LoadBMP_custom("imagemodel.bmp");
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint mesh = CreateBuffer();
	GLuint shader = CreateShader("vs.vert", "fs.frag");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(shader, "MVP");
	glm::mat4 mvp = ProjectionViewModel();

	do 
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawMesh(mesh, shader);
		// Send our transformation to the currently bound shader, in the "MVP" uniform
		// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &mesh);
	glDeleteProgram(shader);
	return 0;
}

int main() 
{
	GLFWwindow* window = OpenGLWindow();
	if (window == nullptr) return -2;
	glewInit();
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	
	return Render(window);
}