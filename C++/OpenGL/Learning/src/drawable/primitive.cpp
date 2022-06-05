#include "../headers/drawable/primitive.h"

namespace drawable::primitive
{
	Point3D::Point3D(const Point3D& point)
		:x(point.x), y(point.y), z(point.z)
	{}

	Point3D::Point3D(const glm::vec3& vec)
		:x(vec.x), y(vec.y), z(vec.z)
	{}

	Point3D::Point3D(float x, float y, float z)
		: x(x), y(y), z(z)
	{}

	Triangle3D::Triangle3D(const Point3D& point1, const Point3D& point2, const Point3D& point3)
	{
		this->points[0] = point1;
		this->points[1] = point2;
		this->points[2] = point3;
	}

	Point2D::Point2D(const Point2D& point)
		:x(point.x), y(point.y)
	{}

	Point2D::Point2D(float x, float y)
		: x(x), y(y)
	{}
}
