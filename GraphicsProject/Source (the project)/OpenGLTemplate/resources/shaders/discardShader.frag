#version 400 core

in vec3 vColour;	
in vec2 vTexCoord;
in vec3 vNormal; 

uniform vec3 vlightDirection;
uniform float t;

out vec4 vOutputColor;
uniform sampler2D sampler0; 


void main()
{
	// Read texel (pixel from image)
	vec4 vTexColour = texture(sampler0, vTexCoord);
	if (vTexColour.r < fract(t/8500)) //compute the fractional part of the argument
		discard;

	// Some shading
	float fAmbientIntensity = 0.15f;
	float fDiffuseIntensity = max(0.0, dot(vNormal, normalize(vlightDirection)));
	float fMult = clamp(fAmbientIntensity+fDiffuseIntensity, 0.0, 1.0);

	// Combine shading with texture colour
	vOutputColor = fMult*vTexColour;

}