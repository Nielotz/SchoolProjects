#pragma once

#include <glad/glad.h>

namespace drawable::primitive
{
	struct Point3D
	{
		GLfloat x = 0;
		GLfloat y = 0;
		GLfloat z = 0;

		Point3D(const Point3D& point);
		Point3D(float x = 0, float y = 0, float z = 0);
	};

	struct Triangle3D
	{
		Point3D points[3];

		Triangle3D(const Point3D& point1, const Point3D& point2, const Point3D& point3);
	};
}
