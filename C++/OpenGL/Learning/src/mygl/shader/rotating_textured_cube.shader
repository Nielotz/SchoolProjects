<VertexShader>
#version 330 core
layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec2 texturePosition;

out vec2 v_texturePosition;

uniform mat4 u_transformation;
uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * u_transformation * vertexPosition;
	v_texturePosition = texturePosition;
};

<FragmentShader>
#version 330 core
layout(location = 0) out vec4 color;

in vec2 v_texturePosition;

uniform sampler2D u_texture;

void main()
{
	color = texture(u_texture, v_texturePosition) + vec4(v_texturePosition.xy, 0, 0);
	// color = texture(u_texture, v_texturePosition);
};