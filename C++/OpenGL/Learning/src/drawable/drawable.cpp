#include "../headers/drawable/drawable.h"
#include "../headers/color.h"

namespace drawable
{
	namespace shape3d
	{
		using primitive::Point3D;

		std::vector<Point3D> Hexahedron::getPoints() const
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
				verticeXY, vertice, verticeX,

				// Right.
				verticeXYZ, verticeXY, verticeX,
				verticeXYZ, verticeX, verticeXZ,

				// Back.
				verticeYZ, verticeXYZ, verticeXZ,
				verticeYZ, verticeXZ, verticeZ,

				// Left.
				verticeY, verticeYZ, verticeZ,
				verticeY, verticeZ, vertice,

				// Bottom.
				verticeX, vertice, verticeZ,
				verticeX, verticeZ, verticeXZ,

				// Top.
				verticeY, verticeXY, verticeXYZ,
				verticeY, verticeXYZ, verticeYZ,
			};
		}

		std::vector<Vertice3D> Hexahedron::getVertices() const
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

			typedef primitive::Point2D Point2D;
			const Point2D& textureRightTop = { 1., 1. };
			const Point2D& textureLeftTop = { 0., 1. };
			const Point2D& textureLeftDown = { 0., 0. };
			const Point2D& textureRightDown = { 1., 0. };

			// Hexahedron faces.
			struct Face
			{
				const Point3D& rightTop;
				const Point3D& leftTop;
				const Point3D& leftDown;
				const Point3D& rightDown;
			};

			const std::vector<Face>& faces = {
				{ verticeXY, verticeY, vertice, verticeX },      // front
				{ verticeXYZ, verticeXY, verticeX, verticeXZ },  // right
				{ verticeYZ, verticeXYZ, verticeXZ, verticeZ },  // back
				{ verticeY, verticeYZ, verticeZ, vertice },      // left
				{ verticeX, vertice, verticeZ, verticeXZ },      // bottom
				{ verticeY, verticeXY, verticeXYZ, verticeYZ },  // top
			};

			std::vector<Vertice3D> vertices;
			vertices.reserve(3 * 8);

			for (int i = 0; i < faces.size(); i++)
			{
				// Upper triangle.
				vertices.emplace_back(Vertice3D{ faces[i].rightTop, textureRightTop });
				vertices.emplace_back(Vertice3D{ faces[i].leftTop, textureLeftTop });
				vertices.emplace_back(Vertice3D{ faces[i].leftDown, textureLeftDown });

				// Bottom triangle.
				vertices.emplace_back(Vertice3D{ faces[i].rightTop, textureRightTop });
				vertices.emplace_back(Vertice3D{ faces[i].leftDown, textureLeftDown });
				vertices.emplace_back(Vertice3D{ faces[i].rightDown, textureRightDown });
			}

			return vertices;
		}

		Point3D Hexahedron::getCenter() const
		{
			const float& halfOfSide = this->sideLength / 2.;
			return Point3D(this->position.x - halfOfSide,
				this->position.y - halfOfSide,
				this->position.z + halfOfSide);
		}

		Hexahedron::Hexahedron(float sideLength, Point3D position, color::RGBA color, float specularStrength)
		{
			this->sideLength = sideLength;
			this->color = color;
			this->position = position;
			this->specularStrength = specularStrength;
		}

		color::RGBA Shape::getColor() const
		{
			return this->color;
		}

		float Shape::getSpecularStrength() const
		{
			return this->specularStrength;
		}

		// TODO [FIX]: center is not a center
		Point3D Shape::getCenter() const
		{
			return Point3D(this->position.x,
				this->position.y,
				this->position.z);
		}
	}
	namespace lighting3d
	{
		LightSource3D::LightSource3D(float luminosity) : luminosity(luminosity)
		{}

		LightSourceHexahedron::LightSourceHexahedron(float sideLength, Point3D position, color::RGBA color, float luminosity)
			: shape3d::Hexahedron(sideLength, position, color),
			LightSource3D(luminosity)
		{};
	}
}
