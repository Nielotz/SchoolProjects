<VertexShader>
#version 330 core
layout(location = 0) in vec3 position;

void main()
{
	gl_Position = vec4(position.x, position.y, position.z, 1.0);
};

<FragmentShader>
#version 330 core
layout(location = 0) out vec4 color;

uniform vec4 u_colorRGBA;

void main()
{
	color = u_colorRGBA;
};