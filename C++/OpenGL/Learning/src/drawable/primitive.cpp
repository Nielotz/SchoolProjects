#include "../headers/drawable/primitive.h"

drawable::primitive::Point3D::Point3D(const Point3D& point)
	:x(point.x), y(point.y), z(point.z)
{}

drawable::primitive::Point3D::Point3D(float x, float y, float z)
	: x(x), y(y), z(z)
{}

drawable::primitive::Triangle3D::Triangle3D(const Point3D& point1, const Point3D& point2, const Point3D& point3)
{
	this->points[0] = point1;
	this->points[1] = point2;
	this->points[2] = point3;
}
