#version 400 core

out vec4 vOutpoutColour;

in vec2 texCoords;

uniform sampler2D texture_diffuse1;

void main() 
{
	vOutputColour = texture(texture_diffuse1, texCoords);
}