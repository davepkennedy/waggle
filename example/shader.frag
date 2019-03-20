#version 440 core

// Interpolated values from the vertex shaders
in vec3 fragmentColor;
in vec3 normal;
in vec3 fragPos;

// Ouput data
out vec3 color;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform vec3 lightPos;

void main(){

    // Output color = color specified in the vertex shader,
    // interpolated between all 3 surrounding vertices

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    color = objectColor * (ambient + diffuse);
}