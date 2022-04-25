#include "../headers/shape/complex.h"
#include "../headers/mygl/debug/debug.h"

namespace complex2d::shape
{
	void ComplexShape::setVisibility(const bool isVisible)
	{
		this->visible = isVisible;
	}

	bool ComplexShape::isVisible() const
	{
		return this->visible;
	}

	/*
	void ComplexShape::printCoordinates()
	{
		using std::cout;
		using std::endl;

		cout << "Printing vertices of " << this->amountOfTriangles << " triangles:" << endl;
		for (int triangleIdx = 0; triangleIdx < this->amountOfTriangles; triangleIdx++)
		{
			cout << "\tTriangle " << triangleIdx << ": \n";

			for (int verticeIdx = 0; verticeIdx < 3; verticeIdx++)
			{
				cout << "\t\tVertice " << verticeIdx << ": ";
				cout << this->triangles[triangleIdx].vertices[verticeIdx].x
					<< " " << this->triangles[triangleIdx].vertices[verticeIdx].y
					<< " " << this->triangles[triangleIdx].vertices[verticeIdx].z;
				cout << endl;
			}
		}
	}
	*/

	/*
	primitive::Vertices Circle::convertToVertices() const
	{
		// Structure of vertices values array:
		//
		//                     triangleOffset-|
		//                                    |
		//     NAME: --------TRIANGLE1--------; ------------TRIANGLE2-------------;
		//
		//                   |-triangleVerticeOffset
		//                   |
		//     NAME: --V1---  --V2---  --V3---; ----V1----  ----V2----  ----V3----;
		//     CORD: X, Y, Z; X, Y, Z; X, Y, Z; X,  Y,  Z;  X,  Y,  Z;  X,  Y,  Z,;
		//     IDX:  0, 1, 2, 3, 4, 5, 6, 7, 8; 9, 10, 11, 12, 13, 14, 15, 16, 17,;

		const size_t amountOfVertices = this->amountOfTriangles * kVerticesPerTriangle;

		const size_t amountOfVertices = amountOfVertices * kCoordinatesPerVertice;

		primitive::Vertices vertices = { new float[amountOfVertices], amountOfVertices };

		for (size_t triangleIdx = 0; triangleIdx < this->amountOfTriangles; triangleIdx++)
		{
			const size_t triangleOffset = triangleIdx * kCoordinatesPerTriangle;
			for (size_t triangleVerticeIdx = 0; triangleVerticeIdx < 3; triangleVerticeIdx++)
			{
				const size_t triangleVerticeOffset = triangleVerticeIdx * kCoordinatesPerVertice;
				const size_t totalVerticeOffset = triangleOffset + triangleVerticeOffset;

				vertices.vertices[totalVerticeOffset] = this->triangles[triangleIdx].vertices[triangleVerticeIdx].x;
				vertices.vertices[totalVerticeOffset + 1] = this->triangles[triangleIdx].vertices[triangleVerticeIdx].y;
				vertices.vertices[totalVerticeOffset + 2] = this->triangles[triangleIdx].vertices[triangleVerticeIdx].z;
			}
		}
		return vertices;
	}
	*/

	color::RGB ComplexShape::getColorRGB() const
	{
		return this->colorRGB;
	}

	/*
	/// @brief Calculate triangles that build circle.
	void ComplexShape::calculateTriangles()
	{
		const primitive::Point3d& centerVertice = this->position;

		// Adjust for non-square window.
		// TODO: add proper x and y scaling.
		float yScale = float(config::kScreenWidth) / float(config::kScreenHeight);

		primitive::Point3d previousEdgePoint = { centerVertice.x + radius , centerVertice.y };
		for (int trianglesIdx = 0; trianglesIdx < this->amountOfTriangles; trianglesIdx++)
		{
			float currentAngle = float(2.0 * std::numbers::pi * (trianglesIdx + 1) / this->amountOfTriangles);

			this->triangles[trianglesIdx].vertices[0] = centerVertice;
			this->triangles[trianglesIdx].vertices[1] = previousEdgePoint;

			this->triangles[trianglesIdx].vertices[2] = {
				radius * cosf(currentAngle) + centerVertice.x, // x
				radius * sinf(currentAngle) * yScale + centerVertice.y  // y
			};
			previousEdgePoint = this->triangles[trianglesIdx].vertices[2];
		}
	}
	*/

	std::shared_ptr<primitive2d::Vertices> Circle::getVertices()
	{
		using namespace primitive2d::shape;

		const size_t amountOfVertices = this->amountOfTriangles * kVerticesPerTriangle;
		std::shared_ptr<primitive2d::Vertices> vertices(new primitive2d::Vertices(amountOfVertices));

		std::shared_ptr<Triangles> triangles = this->convertToTriangles();

		ASSERT(triangles->amount == this->amountOfTriangles);

		for (size_t triangleIdx = 0; triangleIdx < this->amountOfTriangles; triangleIdx++)
		{
			const size_t verticeOffset = triangleIdx * kVerticesPerTriangle;
			vertices->vertices[verticeOffset] = triangles->triangles[triangleIdx].vertices[0];
			vertices->vertices[verticeOffset + 1] = triangles->triangles[triangleIdx].vertices[1];
			vertices->vertices[verticeOffset + 2] = triangles->triangles[triangleIdx].vertices[2];
		}
		return vertices;
	}

	const std::shared_ptr<primitive2d::shape::Triangles> Circle::convertToTriangles() const
	{
		using namespace primitive2d;
		using namespace primitive2d::shape;

		// Adjust for non-square window.
		// TODO: add proper x and y scaling.
		float yScale = float(config::kScreenWidth) / float(config::kScreenHeight);

		std::shared_ptr<Triangles> triangles(new Triangles(this->amountOfTriangles));

		// const Point& circleCenterOffset = Point{ -this->radius, this->radius };
		const Point& circleCenterVertice = this->position;// +circleCenterOffset;
		// 
		Point previousEdgePoint = { circleCenterVertice.x + radius , circleCenterVertice.y };

		for (int trianglesIdx = 0; trianglesIdx < this->amountOfTriangles; trianglesIdx++)
		{
			Vertex& triangleVertice0 = triangles->triangles[trianglesIdx].vertices[0];
			Vertex& triangleVertice1 = triangles->triangles[trianglesIdx].vertices[1];
			Vertex& triangleVertice2 = triangles->triangles[trianglesIdx].vertices[2];

			float currentAngle = float(2.0 * std::numbers::pi * (trianglesIdx + 1) / this->amountOfTriangles);

			triangleVertice0.coords = circleCenterVertice;
			triangleVertice1.coords = previousEdgePoint;

			triangleVertice2.coords = {
				radius * cosf(currentAngle) + circleCenterVertice.x, // x
				radius * sinf(currentAngle) * yScale + circleCenterVertice.y  // y
			};
			previousEdgePoint = triangles->triangles[trianglesIdx].vertices[2].coords;
		}
		return triangles;
	}

	Circle::Circle(const size_t amountOfTriangles, const float radius, const primitive2d::Point position)
	{
		updateCircle(amountOfTriangles, radius, position);
	}

	float Circle::getRadius() const
	{
		return this->radius;
	}

	void Circle::updateCircle(const primitive2d::Point& position)
	{
		this->position = position;
	}

	void Circle::updateCircle(const size_t amountOfTriangles)
	{
		this->amountOfTriangles = amountOfTriangles;
	}

	void Circle::updateCircle(const size_t amountOfTriangles, const float radius)
	{
		this->radius = radius;
		updateCircle(amountOfTriangles);
	}

	void Circle::updateCircle(const size_t amountOfTriangles, const float radius, const primitive2d::Point& position)
	{
		this->radius = radius;
		this->position = position;
		updateCircle(amountOfTriangles);
	}

	void Circle::updateCircle(color::RGB& color)
	{
		this->colorRGB = color;
	}

	std::shared_ptr<primitive2d::Vertices> Triangle::getVertices()
	{
		using primitive2d::Vertices;
		std::shared_ptr<Vertices> vertices(new Vertices(3));
		vertices->vertices[0].coords = { this->position.x + this->sideLength / 2, this->position.y };
		vertices->vertices[1].coords = { this->position.x, this->position.y - sqrtf(3)/2 * this->sideLength };
		vertices->vertices[2].coords = { this->position.x + this->sideLength, this->position.y - sqrtf(3)/2 * this->sideLength };
		return vertices;
	}

	Triangle::Triangle(float sideLength, primitive2d::Point position, color::RGB colorRGB)
		:sideLength(sideLength)
	{
		this->position = position;
		this->colorRGB = colorRGB;
	}
}