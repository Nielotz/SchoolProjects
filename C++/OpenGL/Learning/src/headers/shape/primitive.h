#pragma once

namespace shape::primitive
{
	struct Point
	{
		float x = 0;
		float y = 0;
		float z = 0;
	};

	struct Triangle
	{
		Point vertices[3];
	};
}