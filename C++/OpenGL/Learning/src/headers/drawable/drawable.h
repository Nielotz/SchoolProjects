#pragma once

#include <iostream>
#include <numbers>
#include <vector>

#include "primitive.h"
#include "../consts.h"
#include "../config.h"
#include "../color.h"

namespace drawable
{
	using drawable::primitive::Point3D;
	using drawable::primitive::Triangle3D;
	using drawable::primitive::Vertice3D;
	namespace shape3d
	{
		struct Shape
		{
		public:
			/// <summary>
			/// Generate vector of points. 
			/// First vertice have to start at left bottom corner.
			/// Points have to be provided in counter-clockwise order.
			/// </summary>
			/// <returns>vector of points</returns>
			virtual std::vector<Point3D> getPoints() const = 0;

			// TODO [EVERYTHING]: Find better way to apply textures.
			virtual std::vector<Vertice3D> getVertices() const = 0;
			color::RGBA getColor() const;
			float getSpecularStrength() const;

			virtual Point3D getCenter() const;


		protected:
			/// <summary>
			/// Position of front-left-bottom point.
			/// </summary>
			Point3D position;
			color::RGBA color;
			float specularStrength;
		};

		class Hexahedron : public Shape
		{
		public:
			/// <summary>
			/// Generate vector of points. 
			/// Points are provided in counter-clockwise order starting at right top corner.
			/// </summary>
			/// <returns>vector of points</returns>
			std::vector<Point3D> getPoints() const override;

			/// <summary>
			/// Generate vector of vertices (points with texture coordinates). 
			/// Vertices are provided in counter-clockwise order starting at right top corner.
			/// </summary>
			/// <returns>vector of vertices</returns>
			std::vector<Vertice3D> getVertices() const override;

			Point3D getCenter() const override;

			Hexahedron(float sideLength, Point3D position = { 0.f, 0.f, 0.f }, color::RGBA color = color::kRedRGBA, float specularStrength = 0.5f);

		private:
			float sideLength;
		};
	}

	namespace lighting3d
	{
		class LightSource3D
		{
			float luminosity;
		public:
			LightSource3D(float luminosity);
		};

		class LightSourceHexahedron : public LightSource3D, public shape3d::Hexahedron
		{
		public:
			LightSourceHexahedron(
				float sideLength, Point3D position = { 0.f, 0.f, 0.f }, color::RGBA color = color::kRedRGBA,
				float luminosity = 1.);
		};
	}
}