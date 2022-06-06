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

uniform vec4 u_color;

void clampVec4(inout vec4 vec)
{
	vec.r = clamp(vec.r, 0., 1.);
	vec.g = clamp(vec.b, 0., 1.);
	vec.b = clamp(vec.b, 0., 1.);
	vec.a = clamp(vec.a, 0., 1.);
}

void main()
{
	out_color = u_color;
	clampVec4(out_color);
};