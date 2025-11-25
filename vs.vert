#version 440 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexColor;
layout(location = 3) in vec2 uv;

// Output data ; will be interpolated for each fragment.
layout(location = 0) out vec4 fragPos;
layout(location = 1) out vec2 UV;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec3 color;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main() 
{
    // Output position of the vertex, in clip space : MVP * position
    //gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
    fragPos = MVP * vec4(vertexPosition_modelspace,1);
    // UV of the vertex. No special space for this one.
    UV = uv;
    color = vertexColor;
    normal = normalize(vertexNormal);
    gl_Position = fragPos;
}