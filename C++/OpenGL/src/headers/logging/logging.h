#pragma once
#include <iostream>

namespace logging
{
	void error(const std::string& category, const std::string& message);

	void error(const std::string& message);

	void warning(const std::string& category, const std::string& message);

	void info(const std::string& category, const std::string& message);

	template <typename T>
	void info(const std::string& category, const T& data)
	{
		std::cout << "[INFO] " << "(" << category << ") " << data << std::endl;
	}
}