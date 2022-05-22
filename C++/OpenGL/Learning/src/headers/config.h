#pragma once

#include <string>

namespace config
{
	constexpr unsigned int kScreenWidth = 800;
	constexpr unsigned int kScreenHeight = 800;

	constexpr unsigned int kScreenCenterHorizontal = kScreenWidth / 2;
	constexpr unsigned int kScreenCenterVertical = kScreenHeight / 2;

	const std::string kWindowName = "";

	namespace control
	{
		constexpr float kMoveSpeed = 1.f;  // Units per second. 
		constexpr float kMouseSensitivity = 0.1f;  // Sth per sth else
	}
}