#include "../headers/drawable/shape3d.h"
#include "../headers/color.h"

namespace drawable::shape3d
{
	using primitive::Point3D;

	std::vector<Point3D> Hexahedron::getVertices() const
	{
		// Hexahedron vertices.
		const Point3D& vertice = { position.x,              position.y,              position.z };
		const Point3D& verticeX = { position.x + sideLength, position.y,              position.z };
		const Point3D& verticeY = { position.x,              position.y + sideLength, position.z };
		const Point3D& verticeXY = { position.x + sideLength, position.y + sideLength, position.z };
		const Point3D& verticeZ = { position.x,              position.y,              position.z + sideLength };
		const Point3D& verticeXZ = { position.x + sideLength, position.y,              position.z + sideLength };
		const Point3D& verticeYZ = { position.x,              position.y + sideLength, position.z + sideLength };
		const Point3D& verticeXYZ = { position.x + sideLength, position.y + sideLength, position.z + sideLength };

		// Organize into triangles vertices.
		return {
			vertice, verticeXY, verticeX,
			vertice, verticeXY, verticeY,

			vertice, verticeXZ, verticeX,
			vertice, verticeXZ, verticeZ,

			vertice, verticeYZ, verticeY,
			vertice, verticeYZ, verticeZ,

			verticeXYZ, verticeX, verticeXZ,
			verticeXYZ, verticeX, verticeXY,

			verticeXYZ, verticeY, verticeYZ,
			verticeXYZ, verticeY, verticeXY,

			verticeXYZ, verticeZ, verticeYZ,
			verticeXYZ, verticeZ, verticeXZ,
		};

	}

	Hexahedron::Hexahedron(float sideLength, Point3D position, color::RGBA color)
	{
		this->sideLength = sideLength;
		this->position = position;
		this->color = color;
	}


	color::RGBA Shape::getColor() const
	{
		return this->color;
	}

}