<VertexShader>
#version 330 core
layout(location = 0) in vec4 in_vertexPosition;

void main()
{
	gl_Position = in_vertexPosition;
};


<FragmentShader>
#version 330 core
layout(location = 0) out vec4 out_color;

void clampVec3(inout vec3 vec)
{
	vec.r = clamp(vec.x, 0., 1.);
	vec.g = clamp(vec.y, 0., 1.);
	vec.b = clamp(vec.z, 0., 1.);
}

void main()
{
	out_color = vec4(v_texturePosition.xy, 0, 0);
	clampVec4(out_color);
};