#version 330 core

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
in vec2 UV;
in vec3 normal;
in vec3 fragPos;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform AmbientLight myAmbientLight;
uniform int numLights;
uniform PointLight PointLights[32];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos) 
{
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 ambient = light.color * myAmbientLight.Color; // Assuming a material struct
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture( myTextureSampler, UV ).rgb;
    vec3 reflectDir = reflect(-lightDir, normal);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    return (ambient + diffuse) * attenuation;
}

void main()
{
    color = vec3(0);
    for (int i = 0; i < numLights; i++)
    {
        color += CalcPointLight(PointLights[i], normal, fragPos);
    }
    //color = texture( myTextureSampler, UV ).rgb * myAmbientLight.Color;
}