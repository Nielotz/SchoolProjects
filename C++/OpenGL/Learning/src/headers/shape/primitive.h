#pragma once

#include <glad/glad.h>

namespace shape::primitive
{
	struct Point
	{
		GLfloat x = 0;
		GLfloat y = 0;
		GLfloat z = 0;
	};

	struct Triangle
	{
		Point vertices[3];
	};
}