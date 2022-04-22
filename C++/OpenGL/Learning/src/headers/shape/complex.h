#pragma once

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
		shape::primitive::Point position;
		bool isVisible = true;

	public:
		size_t amountOfTriangles = 0;

		void setVisibility(const bool isVisible);
		bool getVisibility();
		void printCoordinates();
		Coordinates convertToCoordinates() const;
	};

	class Circle : public ComplexShape
	{
		float radius;
		color::RGB color_rgb;

	public:
		float getRadius() const;
		color::RGB getColorRGB() const;

		/// @brief Calculate triangles that build circle.
		void calculateTriangles();

		Circle(const size_t amountOfTriangles, const float radius, const shape::primitive::Point position = { 0,0,0 });

		void updateCircle(const shape::primitive::Point& position);

		void updateCircle(const size_t amountOfTriangles);

		void updateCircle(const size_t amountOfTriangles, const float radius);

		void updateCircle(const size_t amountOfTriangles, const float radius, const shape::primitive::Point& position);

		void updateCircle(color::RGB& color);

		~Circle();
	};
}