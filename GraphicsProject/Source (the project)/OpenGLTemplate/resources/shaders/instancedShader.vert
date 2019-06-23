#version 400 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;

out vec2 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = projections * view * instanceMatrix * vec4(aPos, 1.0);
	texCoords = aTexCoords;
}