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
		const Point3D& verticeZ = { position.x,              position.y,              position.z - sideLength };
		const Point3D& verticeXZ = { position.x + sideLength, position.y,              position.z - sideLength };
		const Point3D& verticeYZ = { position.x,              position.y + sideLength, position.z - sideLength };
		const Point3D& verticeXYZ = { position.x + sideLength, position.y + sideLength, position.z - sideLength };

		// Organize into triangles vertices.
		return {
			// Front.
			verticeXY, verticeY, vertice,
			verticeX, verticeXY, vertice,

			// Right.
			verticeXYZ, verticeXY, verticeX,
			verticeXZ, verticeXYZ, verticeX,

			// Back.
			verticeYZ, verticeXYZ, verticeXZ,
			verticeZ, verticeYZ, verticeXZ,

			// Left.
			verticeY, verticeYZ, verticeZ,
			vertice, verticeY, verticeZ,

			// Bottom.
			verticeX, vertice, verticeZ,
			verticeXZ, verticeX, verticeZ,

			// Top.
			verticeY, verticeXY, verticeXYZ,
			verticeYZ, verticeY, verticeXYZ,
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