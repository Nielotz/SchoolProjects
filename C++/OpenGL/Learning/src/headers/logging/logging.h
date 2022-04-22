#pragma once
#include <iostream>

namespace logging
{
	void error(const std::string& category, const std::string& errorMessage);

	void error(const std::string& errorMessage);

	void warning(const std::string& category, const std::string& errorMessage);

}