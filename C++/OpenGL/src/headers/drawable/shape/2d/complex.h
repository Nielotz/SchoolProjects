#pragma once
/*
#include <iostream>
#include <numbers>

#include "primitive.h"
#include "../consts.h"
#include "../config.h"
#include "../color.h"

namespace complex2d
{
	namespace shape
	{
		struct ComplexShape
		{
		protected:
			// Position of left top corner of the shape.
			primitive2d::Point position;
			bool visible = true;
			color::RGB colorRGB;

		public:
			virtual std::shared_ptr<primitive2d::Vertices> getPoints() = 0;
			color::RGB getColorRGB() const;
			void setVisibility(const bool isVisible);
			bool isVisible() const;
		};

		class Triangle : public ComplexShape
		{
			float sideLength;
		public:
			std::shared_ptr<primitive2d::Vertices> getPoints() override;
			Triangle(float sideLength, primitive2d::Point position, color::RGB colorRGB);

		};

		class Circle : public ComplexShape
		{
			float radius;
			size_t amountOfTriangles;
			const std::shared_ptr<primitive2d::shape::Triangles> convertToTriangles() const;
			
		public:
			std::shared_ptr<primitive2d::Vertices> getPoints() override;

			float getRadius() const;

			Circle(const size_t amountOfTriangles, const float radius, const primitive2d::Point position = { 0,0 });

			// TODO [CODE REFACTOR]:
			//    - prettify tHiS
			void updateCircle(const primitive2d::Point& position);

			void updateCircle(const size_t amountOfTriangles);

			void updateCircle(const size_t amountOfTriangles, const float radius);

			void updateCircle(const size_t amountOfTriangles, const float radius, const primitive2d::Point& position);

			void updateCircle(color::RGB& color);
		};
	}
}*/