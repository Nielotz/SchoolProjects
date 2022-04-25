#pragma once

#include <string>

namespace config
{
	constexpr unsigned int kScreenWidth = 800;
	constexpr unsigned int kScreenHeight = 800;
	const std::string kWindowName = "";

	namespace path
	{
		const std::string shaders = "src/mygl/shader/transforming_triangles.shader";
	}
}