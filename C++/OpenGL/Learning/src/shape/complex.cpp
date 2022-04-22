#include "../headers/shape/complex.h"

namespace shape::complex
{
	void ComplexShape::setVisibility(const bool isVisible)
	{
		this->isVisible = isVisible;
	}

	bool ComplexShape::getVisibility()
	{
		return this->isVisible;
	}

	void ComplexShape::printCoordinates()
	{
		using std::cout;
		using std::endl;

		cout << "Printing coordinates of " << this->amountOfTriangles << " triangles:" << endl;
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

	Coordinates ComplexShape::convertToCoordinates() const
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

		const size_t amountOfCoordinates = amountOfVertices * kCoordinatesPerVertice;

		Coordinates coordinates = { new float[amountOfCoordinates], amountOfCoordinates };

		for (size_t triangleIdx = 0; triangleIdx < this->amountOfTriangles; triangleIdx++)
		{
			const size_t triangleOffset = triangleIdx * kCoordinatesPerTriangle;
			for (size_t triangleVerticeIdx = 0; triangleVerticeIdx < 3; triangleVerticeIdx++)
			{
				const size_t triangleVerticeOffset = triangleVerticeIdx * kCoordinatesPerVertice;
				const size_t totalVerticeOffset = triangleOffset + triangleVerticeOffset;

				coordinates.coordinates[totalVerticeOffset] = this->triangles[triangleIdx].vertices[triangleVerticeIdx].x;
				coordinates.coordinates[totalVerticeOffset + 1] = this->triangles[triangleIdx].vertices[triangleVerticeIdx].y;
				coordinates.coordinates[totalVerticeOffset + 2] = this->triangles[triangleIdx].vertices[triangleVerticeIdx].z;
			}
		}
		return coordinates;
	}

	float Circle::getRadius() const
	{
		return this->radius;
	}

	color::RGB Circle::getColorRGB() const
	{
		return this->color_rgb;
	}

	/// @brief Calculate triangles that build circle.
	void Circle::calculateTriangles()
	{
		const shape::primitive::Point& centerVertice = this->position;

		// Adjust for non-square window.
		// TODO: add proper x and y scaling.
		float yScale = float(config::kScreenWidth) / float(config::kScreenHeight);

		shape::primitive::Point previousEdgePoint = { centerVertice.x + radius , centerVertice.y };
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

	Circle::Circle(const size_t amountOfTriangles, const float radius, const shape::primitive::Point position)
	{
		updateCircle(amountOfTriangles, radius, position);
	}

	void Circle::updateCircle(const shape::primitive::Point& position)
	{
		this->position = position;
		calculateTriangles();
	}

	void Circle::updateCircle(const size_t amountOfTriangles)
	{
		this->amountOfTriangles = amountOfTriangles;

		delete[] this->triangles;
		this->triangles = new shape::primitive::Triangle[amountOfTriangles];
		calculateTriangles();
	}

	void Circle::updateCircle(const size_t amountOfTriangles, const float radius)
	{
		this->radius = radius;
		updateCircle(amountOfTriangles);
	}

	void Circle::updateCircle(const size_t amountOfTriangles, const float radius, const shape::primitive::Point& position)
	{
		this->radius = radius;
		this->position = position;
		updateCircle(amountOfTriangles);
	}

	void Circle::updateCircle(color::RGB& color)
	{
		this->color_rgb = color;
	}

	Circle::~Circle()
	{
		delete[] this->triangles;
	}

}