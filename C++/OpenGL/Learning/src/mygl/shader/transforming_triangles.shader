<VertexShader>
#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 u_model;

void main()
{
	gl_Position = u_model * vec4(position.x, position.y, position.z, 1.0);
};

<FragmentShader>
#version 330 core
layout(location = 0) out vec4 color;

uniform vec4 u_colorRGB;

void main()
{
   color = u_colorRGB;
};