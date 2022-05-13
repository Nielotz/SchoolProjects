#pragma once

#include <string>

namespace config
{
	constexpr unsigned int kScreenWidth = 800;
	constexpr unsigned int kScreenHeight = 800;

	constexpr unsigned int kScreenCenterHorizontal = kScreenWidth / 2;
	constexpr unsigned int kScreenCenterVertical = kScreenHeight / 2;

	const std::string kWindowName = "";

	namespace path
	{
		// const std::string shaders = "src/mygl/shader/transforming_triangles.shader";
		const std::string shaders = "src/mygl/shader/my_first_3d!.shader";
	}

	namespace control
	{
		constexpr float kMoveSpeed = 0.5f;  // Units per second. 
		constexpr float kMouseSensitivity = 0.1f;  // Sth per sth else
	}
}