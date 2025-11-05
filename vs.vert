#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 uv;
out vec4 vecColor;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main() 
{
    gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
    vecColor.xy = uv;
    vecColor.z = vertexColor.z;
    vecColor.w = 1.0f;
}