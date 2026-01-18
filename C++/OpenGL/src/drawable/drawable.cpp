#include "../headers/drawable/drawable.h"
#include "../headers/color.h"

#include <glm/glm.hpp>

namespace drawable
{
	namespace shape3d
	{
		using primitive::Point3D;

		std::vector<Point3D> Hexahedron::getPoints() const
		{
			// Hexahedron vertices.
			const Point3D& point = { position.x,              position.y,              position.z };
			const Point3D& pointX = { position.x + sideLength, position.y,              position.z };
			const Point3D& pointY = { position.x,              position.y + sideLength, position.z };
			const Point3D& pointXY = { position.x + sideLength, position.y + sideLength, position.z };
			const Point3D& pointZ = { position.x,              position.y,              position.z - sideLength };
			const Point3D& pointXZ = { position.x + sideLength, position.y,              position.z - sideLength };
			const Point3D& pointYZ = { position.x,              position.y + sideLength, position.z - sideLength };
			const Point3D& pointXYZ = { position.x + sideLength, position.y + sideLength, position.z - sideLength };

			// Organize into triangles points.
			return {
				// Front.
				pointXY, pointY, point,
				pointXY, point, pointX,

				// Right.
				pointXYZ, pointXY, pointX,
				pointXYZ, pointX, pointXZ,

				// Back.
				pointYZ, pointXYZ, pointXZ,
				pointYZ, pointXZ, pointZ,

				// Left.
				pointY, pointYZ, pointZ,
				pointY, pointZ, point,

				// Bottom.
				pointX, point, pointZ,
				pointX, pointZ, pointXZ,

				// Top.
				pointY, pointXY, pointXYZ,
				pointY, pointXYZ, pointYZ,
			};
		}

		std::vector<Vertice3D> Hexahedron::getVertices() const
		{
			typedef primitive::Point2D Point2D;
			const Point2D& textureRightTop = { 1., 1. };
			const Point2D& textureLeftTop = { 0., 1. };
			const Point2D& textureLeftDown = { 0., 0. };
			const Point2D& textureRightDown = { 1., 0. };

			const std::vector<Face>& faces = this->getFaces();

			std::vector<Vertice3D> vertices;
			vertices.reserve(3 * 2 * 6);  // 3 vertices per triangle, 2 triangles per side, 6 sides per hexahedron

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

		std::pair<std::vector<Vertice3D>, std::vector<Point3D>> Hexahedron::getVerticesWithNormals() const
		{
			typedef primitive::Point2D Point2D;
			const Point2D& textureRightTop = { 1., 1. };
			const Point2D& textureLeftTop = { 0., 1. };
			const Point2D& textureLeftDown = { 0., 0. };
			const Point2D& textureRightDown = { 1., 0. };

			const std::vector<Face>& faces = this->getFaces();

			std::vector<Vertice3D> vertices;
			std::vector<Point3D> normals;
			vertices.reserve(3 * 2 * 6);  // 3 vertices per triangle, 2 triangles per side, 6 sides per hexahedron
			normals.reserve(3 * 2 * 6);  // 1 normal per vertice

			for (const auto& face : faces)
			{
				const auto& faceNormal = face.calculateNormal();
				// Upper triangle.
				vertices.emplace_back(Vertice3D{ face.rightTop, textureRightTop });
				vertices.emplace_back(Vertice3D{ face.leftTop, textureLeftTop });
				vertices.emplace_back(Vertice3D{ face.leftDown, textureLeftDown });

				// Bottom triangle.
				vertices.emplace_back(Vertice3D{ face.rightTop, textureRightTop });
				vertices.emplace_back(Vertice3D{ face.leftDown, textureLeftDown });
				vertices.emplace_back(Vertice3D{ face.rightDown, textureRightDown });

				// Add normals - all are the same = normal to the face.
				for (int i = 0; i < 6; i++)
					normals.emplace_back(faceNormal);
			}

			return { vertices , normals };
		}

		Point3D Hexahedron::getCenter() const
		{
			const float& halfOfSide = this->sideLength / 2.f;
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

		std::vector<Hexahedron::Face> Hexahedron::getFaces() const
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

			return {
				{ verticeXY, verticeY, vertice, verticeX },      // front
				{ verticeXYZ, verticeXY, verticeX, verticeXZ },  // right
				{ verticeYZ, verticeXYZ, verticeXZ, verticeZ },  // back
				{ verticeY, verticeYZ, verticeZ, vertice },      // left
				{ verticeX, vertice, verticeZ, verticeXZ },      // bottom
				{ verticeY, verticeXY, verticeXYZ, verticeYZ },  // top
			};
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
		Point3D Hexahedron::Face::calculateNormal() const
		{
			glm::vec3 point1(this->rightTop.x, this->rightTop.y, this->rightTop.z);
			glm::vec3 point2(this->leftTop.x, this->leftTop.y, this->leftTop.z);
			glm::vec3 point3(this->leftDown.x, this->leftDown.y, this->leftDown.z);

			glm::vec3 vector1 = point2 - point1;
			glm::vec3 vector2 = point3 - point1;

			return Point3D(glm::normalize(glm::cross(vector1, vector2)));
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
