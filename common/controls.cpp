#pragma once
#include <glm/ext/vector_float3.hpp>
#include <GLFW/glfw3.h>
#include "Controls.h"
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

glm::mat4 ComputeMatricesFromInputs(GLFWwindow* window, float deltaTime)
{
	// position
	static glm::vec3 position = glm::vec3(0, 0, 5);
	// horizontal angle : toward -Z
	static float horizontalAngle = 3.14f;
	// vertical angle : 0, look at the horizon
	static float verticalAngle = 0.0f;
	//FoV (Field of view) is the level of zoom. 80° = very wide angle, huge deformations. 60° - 45° : standard. 20° : big zoom.
	float initialFoV = 45.0f;

	float speed = 3.0f; // 3 units / second
	float mouseSpeed = 0.005f;

	// Previous mouse position
	static double xpos = 0, ypos = 0;
	double x, y;

	glfwGetCursorPos(window, &x, &y);
	double dx = x - xpos;
	double dy = y - ypos;
	xpos = x;
	ypos = y;

	// Compute new orientation
	horizontalAngle -= mouseSpeed * deltaTime * dx;
	verticalAngle -= mouseSpeed * deltaTime * dy;

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	// Up vector : perpendicular to both direction and right
	glm::vec3 up = glm::cross(right, direction);

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}

	// Projection matrix : 45&deg; Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(initialFoV), 1024.f/768.f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 ViewMatrix = glm::lookAt(
		position,           // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	return ProjectionMatrix * ViewMatrix;
}
