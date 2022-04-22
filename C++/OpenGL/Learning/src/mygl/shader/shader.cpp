#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <string>

#include "../../../src/headers/mygl/shader/shader.h"
#include "../../../src/headers/mygl/debug/debug.h"
#include "../../headers/config.h"

const std::vector<std::string> MyGLShader::ShaderTypeName =
{
	"Vertex",
	"Fragment",
};

std::string MyGLShader::checkProgramStatus(const GLuint& id, const GLenum& operationType)
{
	GLint success;
	myGLCall(glGetProgramiv(1, operationType, &success));

	if (success != GL_TRUE)
	{
		int errorLength;
		myGLCall(glGetProgramiv(id, GL_INFO_LOG_LENGTH, &errorLength));

		char* infoLog = new char[errorLength];
		glGetProgramInfoLog(id, errorLength, &errorLength, infoLog);
		const std::string stringInfoLog(infoLog);
		delete[] infoLog;

		return stringInfoLog;
	}

	return "";
}

std::string MyGLShader::checkShaderStatus(const GLuint& id, const GLenum& operationType)
{
	GLint success;
	myGLCall(glGetShaderiv(id, operationType, &success));

	if (success != GL_TRUE)
	{
		int errorLength;
		myGLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &errorLength));

		char* infoLog = new char[errorLength];
		glGetShaderInfoLog(id, errorLength, &errorLength, infoLog);
		const std::string stringInfoLog(infoLog);
		delete[] infoLog;

		return stringInfoLog;
	}

	return "";
}

GLuint MyGLShader::compile(const ShaderType& shaderType, const std::string& sourceCode)
{
	GLenum glShaderType = GL_FALSE;
	if (shaderType == ShaderType::Fragment)
		glShaderType = GL_FRAGMENT_SHADER;
	else if (shaderType == ShaderType::Vertex)
		glShaderType = GL_VERTEX_SHADER;
	else
		throw "Unsupporter shader type.";

	myGLCall(GLuint compiledShaderID = glCreateShader(glShaderType));

	const GLchar* c_sourceCode = sourceCode.c_str();
	myGLCall(glShaderSource(compiledShaderID, 1, &c_sourceCode, NULL));
	myGLCall(glCompileShader(compiledShaderID));

	const std::string &errorMessage = checkShaderStatus(compiledShaderID, GL_COMPILE_STATUS);
	if (!errorMessage.empty())
	{
		std::string shaderName = ShaderTypeName[int(shaderType)];
		std::transform(shaderName.begin(), shaderName.end(), shaderName.begin(), ::toupper);
		std::cout << "ERROR::SHADER::" << shaderName << "::COMPILATION_FAILED\n" << errorMessage << std::endl;

		throw;
	}

	return compiledShaderID;
}

std::unordered_map<MyGLShader::ShaderType, std::string> MyGLShader::parseFromFile(const char* filename)
{
	std::ifstream shadersFile(filename, std::ios::in);
	ASSERT(shadersFile.is_open());

	std::unordered_map<MyGLShader::ShaderType, std::string> shaderCode;

	MyGLShader::ShaderType activeShader = MyGLShader::ShaderType::None;
	std::string line;
	while (shadersFile.good() && std::getline(shadersFile, line))
	{
		if (line.contains("<VertexShader>"))
			activeShader = ShaderType::Vertex;
		else if (line.contains("<FragmentShader>"))
			activeShader = ShaderType::Fragment;
		else
			shaderCode[activeShader] += line + "\n";
	}

	return shaderCode;
}

GLuint MyGLShader::createProgram()
{
	const std::string& shaderPath = config::path::shaders;
	std::cout << "Parsing shader file " << shaderPath << "..." << std::endl;
	auto parsedShaders = MyGLShader::parseFromFile(shaderPath.c_str());

	if (parsedShaders.contains(ShaderType::None))
	{
		std::cout << "Missing header. Skipped: " << parsedShaders.at(ShaderType::None) << std::endl;
		parsedShaders.erase(ShaderType::None);
	}

	std::cout << "Creating GPU program..." << std::endl;
	GLuint shaderProgram = glCreateProgram();
	for (const auto& [shaderType, code] : parsedShaders)
	{
		std::cout << "Compiling " << MyGLShader::ShaderTypeName[int(shaderType)] << "..." << std::endl;
		GLuint compiledShader = MyGLShader::compile(shaderType, parsedShaders.at(shaderType));
		myGLCall(glAttachShader(shaderProgram, compiledShader));
		myGLCall(glDeleteShader(compiledShader));
	}

	myGLCall(glLinkProgram(shaderProgram));

	const std::string& errorMessage = checkProgramStatus(shaderProgram, GL_LINK_STATUS);
	if (!errorMessage.empty())
	{
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\nReason: " << errorMessage << std::endl;
		throw;
	}
	return shaderProgram;
}
