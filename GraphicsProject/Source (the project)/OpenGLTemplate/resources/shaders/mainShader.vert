#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

// Vertex colour output to fragment shader -- using Gouraud (interpolated) shading
out vec2 vTexCoord;	// Texture coordinate

out vec3 worldPosition;	// used for skybox
out vec3 eyeNorm;
out vec4 eyePosition;

// This is the entry point into the vertex shader
void main()
{	
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0);

	// Save the world position for rendering the skybox
	worldPosition = inPosition;

	// Pass through the texture coordinate
	vTexCoord = inCoord;

	eyeNorm = normalize(matrices.normalMatrix * inNormal);
	eyePosition = matrices.modelViewMatrix * vec4(inPosition, 1.0f);
}	