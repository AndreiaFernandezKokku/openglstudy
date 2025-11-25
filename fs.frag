#version 440 core

struct AmbientLight
{
    vec3 Color;
};

struct PointLight 
{
    vec3 position;
    vec3 color;
	float constant;
    float linear;
    float quadratic;
};

// Interpolated values from the vertex shaders
layout(location = 0) in vec4 fragPos;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec3 normal;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform AmbientLight myAmbientLight;
uniform int numLights;

layout (std140) uniform Lights 
{
    PointLight PointLights[32];
};

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 Pos) 
{
    vec3 lightDir = normalize(light.position - Pos);
    vec3 ambient = light.color * myAmbientLight.Color; // Assuming a material struct
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture( myTextureSampler, UV ).rgb;
    vec3 reflectDir = reflect(-lightDir, normal);

    float distance = length(light.position - Pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    return (ambient + diffuse) * attenuation;
}

void main()
{
    color = texture( myTextureSampler, UV ).rgb * myAmbientLight.Color;
    for (int i = 0; i < numLights; i++)
    {
        color += clamp(CalcPointLight(PointLights[i], normal, fragPos.xyz), 0.0, 1.0);
    }
}