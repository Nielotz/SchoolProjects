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
			/// First point starts at left bottom corner.
			/// Points are returned in counter-clockwise order.
			/// </summary>
			/// <returns>vector of points</returns>
			virtual std::vector<Point3D> getPoints() const = 0;

			/// <summary>
			/// Generate vector of vectors. 
			/// First vertice starts at left bottom corner.
			/// Vectors are returned in counter-clockwise order.
			/// </summary>
			/// <returns>vector of vectors</returns>
			// TODO [EVERYTHING]: Find better way to apply textures.
			virtual std::vector<Vertice3D> getVertices() const = 0;

			/// <summary>
			/// Generate vector of vectors with normals. 
			/// First vertice starts at left bottom corner.
			/// Vectors are returned in counter-clockwise order.
			/// </summary>
			/// <returns>vector of vectors</returns>
			// TODO [EVERYTHING]: Find better way to apply textures
			virtual std::pair<std::vector<Vertice3D>, std::vector<Point3D>> getVerticesWithNormals() const = 0;

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

			std::pair<std::vector<Vertice3D>, std::vector<Point3D>> getVerticesWithNormals() const override;

			Point3D getCenter() const override;

			Hexahedron(float sideLength, Point3D position = { 0.f, 0.f, 0.f }, color::RGBA color = color::kRedRGBA, float specularStrength = 0.5f);

		private:
			float sideLength;

			struct Face
			{
				const Point3D rightTop;
				const Point3D leftTop;
				const Point3D leftDown;
				const Point3D rightDown;
				Point3D calculateNormal() const;
			};
			std::vector<Face> getFaces() const;
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