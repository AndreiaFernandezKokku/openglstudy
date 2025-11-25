#version 440 core

layout(location = 3) in vec3 color; // Input from vertex shader
layout(location = 0) out vec4 fragColor;


void main()
{
    fragColor = vec4(color, 1.0);
}