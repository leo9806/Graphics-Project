#version 400 core

uniform int levels;

in vec3 vColour;			// Note: colour is smoothly interpolated (default)
out vec4 vOutputColour;
in float fIntensity;

void main()
{	
	//vOutputColour = vec4(glm::vec3(1.0, 0.0, 0.0), 1.0);

	vec3 quantisedColour = floor(vColour*levels) / levels;

	if (fIntensity < 0.5) 
	{
		vOutputColour = vec4(quantisedColour, .50);
	}

	if (fIntensity > 0.5 && fIntensity < 0.75) 
	{
		vOutputColour = vec4(quantisedColour, .50);
	}
	if (fIntensity > 0.75)
	{
		vOutputColour = vec4(quantisedColour, .50);
	}
}
