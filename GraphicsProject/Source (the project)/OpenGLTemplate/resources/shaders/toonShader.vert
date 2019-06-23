#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;


struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

uniform LightInfo light1; 
uniform MaterialInfo material1;
out float fIntensity;

// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

out vec3 vColour;

void main()
{	

	// Normally, one would simply transform the vertex spatial position using 
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0);

	// This code implements the Blinn-Phong reflectance model (to be discussed in Lecture 6)
	// Code based on the OpenGL 4.0 Shading Language Cookbook, pages 92 - 93
	vec3 vEyeNorm = normalize(matrices.normalMatrix * inNormal);
	vec4 vEyePosition = matrices.modelViewMatrix * vec4(inPosition, 1.0);
	vec3 s = normalize(vec3(light1.position - vEyePosition));
	vec3 v = normalize(-vEyePosition.xyz);
	vec3 h = normalize(v + s);
	vec3 n = vEyeNorm;
	vec3 ambientColour = light1.La * material1.Ma;
	float fDiffuseIntensity = max(dot(s, n), 0.0);
	vec3 diffuseColour = light1.Ld * material1.Md * fDiffuseIntensity;
	vec3 specularColour = vec3(0.0);
	float fSpecularIntensity = 0.0;
	if (fDiffuseIntensity > 0.0) {
		fSpecularIntensity = pow(max(dot(h, n), 0.0), material1.shininess);
		specularColour = light1.Ls * material1.Ms * fSpecularIntensity;
	}
	
	vColour = ambientColour + diffuseColour + specularColour;
	fIntensity = fDiffuseIntensity + fSpecularIntensity;
	
} 
