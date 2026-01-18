<VertexShader>
#version 330 core
layout(location = 0) in vec4 vertexPosition;

uniform mat4 u_transformation;
uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * u_transformation * vertexPosition;
};

<FragmentShader>
#version 330 core
layout(location = 0) out vec4 color;

in vec2 v_texturePosition;

uniform vec4 u_color;

void main()
{
	color = u_color;
};