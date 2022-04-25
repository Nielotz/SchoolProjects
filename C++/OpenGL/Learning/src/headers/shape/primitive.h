#pragma once

#include <memory>

namespace primitive2d
{
	struct Point
	{
		float x = 0;
		float y = 0;

		Point operator-(const Point& other) const;
		Point operator+(const Point& other) const;
		Point& operator+=(const Point& other);
	};

	struct Vertex
	{
		Point coords;
		// color::RGB color;
	};

	// TODO [CODE ORGANIZATION]:
	//    - find better place for this dude
	struct Vertices
	{
		size_t amount = 0;
		Vertex* vertices;
		void print();
		Vertices(size_t amount);
		~Vertices();
	};
	namespace shape
	{
		struct Triangle
		{
			Vertex vertices[3];
		};

		// TODO [CODE ORGANIZATION]:
		//    - find better place for this dude
		struct Triangles
		{
			size_t amount = 0;
			Triangle* triangles;
			Triangles(size_t amount);
			~Triangles();
			void print();
		};
	}
}

/*
namespace primitive3d
{
	struct Point3d
	{
		float x = 0;
		float y = 0;
		float z = 0;

		Point3d operator-(const Point3d& other) const;
		Point3d operator+(const Point3d& other) const;
		Point3d& operator+=(const Point3d& other);
	};
	struct Vertex3d
	{
		Point3d coords;
	};

	// TODO [CODE ORGANIZATION]:
	//    - find better place for this dude
	struct Vertices
	{
		size_t amount = 0;
		Vertex3d* vertices = nullptr;
	};
	namespace shape
	{
		struct Triangle
		{
			Vertex3d vertices[3];
		};

		// TODO [CODE ORGANIZATION]:
		//    - find better place for this dude
		struct Triangles
		{
			size_t amount = 0;
			Triangle* triangles = nullptr;
		};
	}
}*/