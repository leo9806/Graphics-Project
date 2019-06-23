#version 400 core

vec3 vColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

out vec4 vOutputColour;		// The output colour
in vec3 eyeNorm;
in vec4 eyePosition;

uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
uniform bool lightsOff;
in vec3 worldPosition;

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	vec3 direction;
	float exponent;
	float cutoff;
};

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

// Lights and materials passed in as uniform variables from client programme
uniform LightInfo light[12];
uniform MaterialInfo material1;

vec3 PhongModel(LightInfo lightSource[12], vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(lightSource[0].position - p));
	vec3 v = normalize(-p.xyz);
	vec3 r = reflect(-s, n);
	vec3 h = normalize(v + s);
	vec3 ambient = lightSource[0].La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = lightSource[0].Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = lightSource[0].Ls * material1.Ms * pow(max(dot(h, n), 0.0f), material1.shininess + eps);
	

	return ambient + diffuse + specular;
}

vec3 BlinnPhongSpotlightModel(LightInfo lightSource, vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(lightSource.position - p));
	float angle = acos(dot(-s, lightSource.direction));
	float cutoff = radians(clamp(lightSource.cutoff, 0.0, 90.0));
	vec3 ambient = lightSource.La * material1.Ma;
	if (angle < cutoff) 
	{
		float spotFactor = pow(dot(-s, lightSource.direction), lightSource.exponent);
		vec3 v = normalize(-p.xyz);
		vec3 h = normalize(v + s);
		float sDotN = max(dot(s, n), 0.0);
		vec3 diffuse = lightSource.Ld * material1.Md * sDotN;
		vec3 specular = vec3(0.0);
		if (sDotN > 0.0)
		{
		specular = lightSource.Ls * material1.Ms * pow(max(dot(h, n), 0.0), material1.shininess);
		}
		
		return ambient + spotFactor * (diffuse + specular);
	} else
		return ambient;
}

void main()
{
	// Apply the Phong model to compute the vertex colour
	if(!lightsOff)
	{
		vColour = PhongModel(light, eyePosition, eyeNorm);
	}
	else 
	{
		for(int i =0; i<12; ++i)
		{
			vColour += BlinnPhongSpotlightModel(light[i], eyePosition, eyeNorm);
		}
		
	}

	if (renderSkybox) {
			vOutputColour = texture(CubeMapTex, worldPosition);

		} else {

			// Get the texel colour from the texture sampler
			vec4 vTexColour = texture(sampler0, vTexCoord);	

			if (bUseTexture)
				vOutputColour = vTexColour*vec4(vColour, 1.0f);	// Combine object colour and texture 
			else
				vOutputColour = vec4(vColour, 1.0f);	// Just use the colour instead
		}
}