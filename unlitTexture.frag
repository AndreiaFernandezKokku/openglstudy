#version 440 core

layout(location = 1) in vec2 UV;

layout(location = 0) out vec4 fragColor;

uniform sampler2D myTextureSampler;

void main()
{
    fragColor = texture( myTextureSampler, UV );
}