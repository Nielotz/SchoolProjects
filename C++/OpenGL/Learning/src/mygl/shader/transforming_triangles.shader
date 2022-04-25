<VertexShader>
#version 330 core
layout(location = 0) in vec2 position;

uniform mat4 u_transformations[4];
uniform int u_transformationsAmount;

void main()
{
	mat4 combinedTransformations;

	if (u_transformationsAmount > 0)
	{
		combinedTransformations = u_transformations[0];

		for (int i = 1; i < u_transformationsAmount; i++)
		{
			combinedTransformations = combinedTransformations * u_transformations[i];
		}

		gl_Position = combinedTransformations * vec4(position.x, position.y, 0., 1.0); //combinedTransformations * vec4(position.x, position.y, 0., 1.0);
	}
	else
	{
		gl_Position = vec4(position.x, position.y, 0., 1.0);
	}

};

<FragmentShader>
#version 330 core
layout(location = 0) out vec4 color;

uniform vec4 u_colorRGBA;

void main()
{
	color = u_colorRGBA;
};