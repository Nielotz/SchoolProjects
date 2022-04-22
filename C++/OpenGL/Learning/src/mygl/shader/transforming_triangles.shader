<VertexShader>
#version 330 core
layout(location = 0) in vec3 position;
//layout(location = 1) in vec3 textureCoordinates;

uniform mat4 u_model;

void main()
{
   gl_Position = u_model * vec4(position.x, position.y, position.z, 1.0);
};

<FragmentShader>
#version 330 core
layout(location = 0) out vec4 color;

uniform vec4 u_colorRGB;
//uniform sampler2D u_texture1;
//uniform sampler2D u_texture2;

void main()
{
//   vec4 = texture(u_texture1, texture1Coordinates);
   color = u_colorRGB;
};