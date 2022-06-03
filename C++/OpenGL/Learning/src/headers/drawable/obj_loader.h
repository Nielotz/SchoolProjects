#pragma once

#include "drawable.h"

#include <string>

namespace obj_loader
{
	typedef drawable::Point3D Point3D;
	typedef drawable::primitive::Point2D Point2D;
	typedef drawable::Vertice3D Vertice3D;
	struct Shape3DFromOBJ : public drawable::shape3d::Shape
	{
		std::vector<Vertice3D> vertices;
		std::vector<Point3D> normals;

		std::vector<Point3D> getPoints() const override;

		std::vector<Vertice3D> getVertices() const override;
	};

	std::shared_ptr<Shape3DFromOBJ> loadFromFile(const std::string& path);
}