#pragma once

#include <iostream>
#include <numbers>
#include <vector>

#include "primitive.h"
#include "../consts.h"
#include "../config.h"
#include "../color.h"

namespace drawable::shape3d
{
	using drawable::primitive::Point3D;
	using drawable::primitive::Triangle3D;
	struct Shape
	{
	protected:
		Point3D position;
		color::RGBA color;
	public:
		virtual std::vector<Point3D> getVertices() const = 0;
		color::RGBA getColor() const;
	};

	class Hexahedron : public Shape
	{

	public:
		std::vector<Point3D> getVertices() const override;

		Hexahedron(float sideLength, Point3D position = { 0.f, 0.f, 0.f }, color::RGBA color = color::kRedRGBA);
	private:
		float sideLength;
	};
}
