<VertexShader>
#version 330 core
layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec2 texturePosition;

uniform mat4 u_transformation;
uniform mat4 u_MVP;

out vec2 v_texturePosition;
out vec3 v_fragmentPosition;

void main()
{
	gl_Position = u_MVP * u_transformation * vertexPosition;
	
	vec4 vertexAfterTransformation = u_transformation * vertexPosition;
	
	v_fragmentPosition = vertexAfterTransformation.xyz;
	
	v_texturePosition = texturePosition;
};


<FragmentShader>
#version 330 core
layout(location = 0) out vec4 out_color;

in vec2 v_texturePosition;
in vec3 v_fragmentPosition;

// Vertex data.
uniform vec3 u_normal;

// Texture.
uniform sampler2D u_texture;

// Observer.
uniform vec3 u_viewPos;

// Lightning.
uniform float u_ambientLightStrength;
uniform vec3 u_ambientLightColor;

uniform vec3 u_lightPosition;
uniform vec3 u_lightColor;
uniform vec3 u_lightSpecularStrength;

uniform int u_isAmbientLight;
uniform int u_isDiffuseLight;
uniform int u_isSpecularLight;

void clampVec3(inout vec3 vec)
{
	vec.r = clamp(vec.x, 0., 1.);
	vec.g = clamp(vec.y, 0., 1.);
	vec.b = clamp(vec.z, 0., 1.);
}

void clampVec4(inout vec4 vec)
{
	vec.r = clamp(vec.x, 0., 1.);
	vec.g = clamp(vec.y, 0., 1.);
	vec.b = clamp(vec.z, 0., 1.);
	vec.a = clamp(vec.a, 0., 1.);
}

float Light_calculateDiffuseCoefficients(in vec3 normalNormalized, in vec3 lightDirectionNormalized)
{
	return max(dot(normalNormalized, lightDirectionNormalized), 0.0);
}

float Light_calculateSpecularCoefficients(in vec3 normalNormalized, in vec3 lightDirectionNormalized, in vec3 viewPosition, in vec3 fragmentPosition)
{
	vec3 viewDirectionNormalized = normalize(viewPosition - fragmentPosition);
	vec3 reflectDirection = reflect(-lightDirectionNormalized, normalNormalized);
	return pow(max(dot(viewDirectionNormalized, reflectDirection), 0.0), 32);
}

void applyLightning(inout vec4 color)
{
	vec3 normalNormalized = normalize(u_normal);
	vec3 lightDirectionNormalized = normalize(u_lightPosition - v_fragmentPosition);

	float ambientCoefficient = u_ambientLightStrength;
	float diffuseCoefficient = Light_calculateDiffuseCoefficients(normalNormalized, lightDirectionNormalized);
	float spectularCoefficient = Light_calculateSpecularCoefficients(normalNormalized, lightDirectionNormalized, u_viewPos, v_fragmentPosition);
	
	vec3 ambientPart = ambientCoefficient * u_ambientLightColor * u_isAmbientLight;
	vec3 diffusePart = diffuseCoefficient * u_lightColor * u_isDiffuseLight;
	vec3 spectularPart = spectularCoefficient * u_lightColor * u_isSpecularLight;

	// Apply
	vec3 lightMultipliers = ambientPart + diffusePart + spectularPart;
	clampVec3(lightMultipliers);
	
	color.rgb *= lightMultipliers;
}

void main()
{
	out_color = texture(u_texture, v_texturePosition) + vec4(v_texturePosition.xy, 0, 0);
	clampVec4(out_color);

	applyLightning(out_color);
	clampVec4(out_color);

};