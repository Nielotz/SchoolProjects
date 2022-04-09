#pragma once

#include <glad/glad.h>
#include <iostream>
#include <numbers>

#include "primitive.h"
#include "../raw/coordinates.h"
#include "../consts.h"
#include "../config.h"
#include "../color.h"

namespace shape::complex
{
	struct ComplexShape
	{
	protected:
		shape::primitive::Triangle* triangles = nullptr;

	public:
		GLsizei amountOfTriangles = 0;

		void printCoordinates()
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

		Coordinates convertToCoordinates() const
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

			const GLsizei amountOfVertices = this->amountOfTriangles * kVerticesPerTriangle;

			const GLsizei amountOfCoordinates = amountOfVertices * kCoordinatesPerVertice;

			Coordinates coordinates = { new GLfloat[amountOfCoordinates], amountOfCoordinates };

			for (GLsizei triangleIdx = 0; triangleIdx < this->amountOfTriangles; triangleIdx++)
			{
				const GLsizei triangleOffset = triangleIdx * kCoordinatesPerTriangle;
				for (GLsizei triangleVerticeIdx = 0; triangleVerticeIdx < 3; triangleVerticeIdx++)
				{
					const GLsizei triangleVerticeOffset = triangleVerticeIdx * kCoordinatesPerVertice;
					const GLsizei totalVerticeOffset = triangleOffset + triangleVerticeOffset;

					coordinates.coordinates[totalVerticeOffset] = this->triangles[triangleIdx].vertices[triangleVerticeIdx].x;
					coordinates.coordinates[totalVerticeOffset + 1] = this->triangles[triangleIdx].vertices[triangleVerticeIdx].y;
					coordinates.coordinates[totalVerticeOffset + 2] = this->triangles[triangleIdx].vertices[triangleVerticeIdx].z;
				}
			}
			return coordinates;
		}
	};

	class Circle : public ComplexShape
	{
		GLfloat radius;
		color::RGB color_rgb;

	public:
		GLfloat getRadius() const
		{
			return this->radius;
		}

		color::RGB getColorRGB() const
		{
			return this->color_rgb;
		}

		/// @brief Calculate triangles that build circle.
		void calculateTriangles()
		{
			const shape::primitive::Point centerVertice = { 0, 0 };

			// Adjust for non-square window.
			// TODO: add proper x and y scaling.
			GLfloat yScale = GLfloat(kScreenWidth) / GLfloat(kScreenHeight);

			shape::primitive::Point previousEdgePoint = { radius, 0 , 0 };
			for (int trianglesIdx = 0; trianglesIdx < this->amountOfTriangles; trianglesIdx++)
			{
				GLfloat currentAngle = GLfloat(2.0 * std::numbers::pi * (trianglesIdx + 1) / this->amountOfTriangles);

				this->triangles[trianglesIdx].vertices[0] = centerVertice;
				this->triangles[trianglesIdx].vertices[1] = previousEdgePoint;

				this->triangles[trianglesIdx].vertices[2] = {
					radius * cosf(currentAngle), // x
					radius * sinf(currentAngle) * yScale  // y
				};
				previousEdgePoint = this->triangles[trianglesIdx].vertices[2];
			}
		}

		Circle(GLsizei amountOfTriangles, GLfloat radius)
		{
			updateCircle(amountOfTriangles, radius);
		}

		void updateCircle(GLsizei amountOfTriangles)
		{
			this->amountOfTriangles = amountOfTriangles;

			delete[] this->triangles;
			this->triangles = new shape::primitive::Triangle[amountOfTriangles];
			calculateTriangles();
		}

		void updateCircle(GLsizei amountOfTriangles, GLfloat radius)
		{
			this->radius = radius;
			updateCircle(amountOfTriangles);
		}

		void updateCircle(color::RGB& color)
		{
			this->color_rgb = color;
		}

		~Circle()
		{
			delete[] this->triangles;
		}
	};
}