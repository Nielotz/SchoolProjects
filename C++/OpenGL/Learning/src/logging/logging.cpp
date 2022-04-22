#include "../../src/headers/logging/logging.h"

void logging::error(const std::string& category, const std::string& errorMessage)
{
	std::cout << "[ERROR] " << "(" << category << ") " << errorMessage << std::endl;
}

void logging::error(const std::string& errorMessage)
{
	std::cout << "[ERROR] " << errorMessage << std::endl;
}

void logging::warning(const std::string& category, const std::string& errorMessage)
{
	std::cout << "[WARNING] " << "(" << category << ") " << errorMessage << std::endl;
}
