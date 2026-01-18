#include "../../src/headers/logging/logging.h"

void logging::error(const std::string& category, const std::string& message)
{
	std::cout << "[ERROR] " << "(" << category << ") " << message << std::endl;
}

void logging::error(const std::string& message)
{
	std::cout << "[ERROR] " << message << std::endl;
}

void logging::warning(const std::string& category, const std::string& message)
{
	std::cout << "[WARNING] " << "(" << category << ") " << message << std::endl;
}

void logging::info(const std::string& category, const std::string& message)
{
	std::cout << "[INFO] " << "(" << category << ") " << message << std::endl;
}
