#include "../../headers/shape/primitive.h"
#include <iostream>

namespace primitive2d
{
	Point Point::operator-(const Point& other) const
	{
		return { this->x - other.x, this->y - other.y };
	}

	Point Point::operator+(const Point& other) const
	{
		return { this->x + other.x, this->y + other.y };
	}

	Point& Point::operator+=(const Point& other)
	{
		this->x += other.x;
		this->y += other.y;

		return *this;
	};

	Vertices::Vertices(size_t amount)
		:amount(amount)
	{
		this->vertices = new Vertex[amount];;
	}

	void Vertices::print()
	{
		for (size_t i = 0; i < this->amount; i++)
			std::cout << this->vertices[i].coords.x << " " << this->vertices[i].coords.y << std::endl;
	}

	
	Vertices::~Vertices()
	{
		delete[] this->vertices;
	}

}

primitive2d::shape::Triangles::Triangles(size_t amount)
	:amount(amount)
{
	this->triangles = new Triangle[amount];
}

primitive2d::shape::Triangles::~Triangles()
{
	delete[] this->triangles;
}

void primitive2d::shape::Triangles::print()
{
	for (size_t i = 0; i < this->amount; i++)
	{
		std::cout << this->triangles[i].vertices[0].coords.x << " " << this->triangles[i].vertices[0].coords.y << std::endl <<
			this->triangles[i].vertices[1].coords.x << " " << this->triangles[i].vertices[1].coords.y << std::endl <<
			this->triangles[i].vertices[2].coords.x << " " << this->triangles[i].vertices[2].coords.y << std::endl;
	}
}
