#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <string>

#include "../../../src/headers/mygl/shader/shader.h"
//#include "../../headers/config.h"
#include "../../headers/mygl/debug/debug.h"

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

GLuint MyGLShader::getShaderProgramID()
{
	return this->shaderProgramID;
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

	const std::string& errorMessage = checkShaderStatus(compiledShaderID, GL_COMPILE_STATUS);
	if (!errorMessage.empty())
	{
		std::string shaderName = ShaderTypeName[int(shaderType)];
		std::transform(shaderName.begin(), shaderName.end(), shaderName.begin(), ::toupper);

		logging::error("Compiling shader program", "ERROR::SHADER::" + shaderName + "::COMPILATION_FAILED\n" + errorMessage);

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

GLuint MyGLShader::createProgramFromFile(const std::string& path)
{
	logging::info("Creating shader program", "Parsing shader file " + path + "...");
	auto parsedShaders = MyGLShader::parseFromFile(path.c_str());

	if (parsedShaders.contains(ShaderType::None))
	{
		std::cout << "Missing header. Skipped: " << parsedShaders.at(ShaderType::None) << std::endl;
		parsedShaders.erase(ShaderType::None);
	}

	logging::info("Creating shader program", "Creating GPU program...");
	this->shaderProgramID = glCreateProgram();
	for (const auto& [shaderType, code] : parsedShaders)
	{
		logging::info("Creating shader program", "Compiling " + MyGLShader::ShaderTypeName[int(shaderType)] + "...");
		GLuint compiledShader = MyGLShader::compile(shaderType, parsedShaders.at(shaderType));
		myGLCall(glAttachShader(this->shaderProgramID, compiledShader));
		myGLCall(glDeleteShader(compiledShader));
	}

	myGLCall(glLinkProgram(this->shaderProgramID));

	const std::string& errorMessage = checkProgramStatus(this->shaderProgramID, GL_LINK_STATUS);
	if (!errorMessage.empty())
	{
		logging::error("Creating shader program", "ERROR::SHADER::PROGRAM::LINKING_FAILED\nReason: " + errorMessage);
		throw;
	}

	return this->shaderProgramID;
}

void MyGLShader::setGLUniform3f(const GLchar* uniformName, GLfloat v0, GLfloat v1, GLfloat v2)
{
	myGLCall(auto u_ID = glGetUniformLocation(this->shaderProgramID, uniformName));

	const auto& log_warning = [uniformName]() {logging::warning("Cannot find uniform", uniformName); };
	ASSERTC(u_ID != -1, log_warning);  // Uniform not found.

	myGLCall(glUniform3f(u_ID, v0, v1, v2));
}

void MyGLShader::setGLUniform3f(const GLchar* uniformName, drawable::primitive::Point3D position)
{
	this->setGLUniform3f(uniformName, position.x, position.y, position.z);
}

void MyGLShader::setGLUniform3f(const GLchar* uniformName, color::RGB color)
{
	this->setGLUniform3f(uniformName, color.red, color.green, color.blue);
}

void MyGLShader::setGLUniform3f(const GLchar* uniformName, glm::vec3 vec)
{
	this->setGLUniform3f(uniformName, vec.x, vec.y, vec.z);
}

void MyGLShader::setGLUniform4f(const GLchar* uniformName, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	myGLCall(auto u_ID = glGetUniformLocation(this->shaderProgramID, uniformName));

	const auto& log_warning = [uniformName]() {logging::warning("Cannot find uniform", uniformName); };
	ASSERTC(u_ID != -1, log_warning);  // Uniform not found.
	

	myGLCall(glUniform4f(u_ID, v0, v1, v2, v3));
}

void MyGLShader::setGLUniform4f(const GLchar* uniformName, color::RGBA color)
{
	this->setGLUniform4f(uniformName, color.red, color.green, color.blue, color.alfa);
}

void MyGLShader::setGLUniform4f(const GLchar* uniformName, color::RGB color)
{
	this->setGLUniform4f(uniformName, color.red, color.green, color.blue, 1.);
}

void MyGLShader::setGLUniform1f(const GLchar* uniformName, GLfloat v0)
{
	myGLCall(auto u_ID = glGetUniformLocation(this->shaderProgramID, uniformName));
	// ASSERT(u_ID != -1);  // Uniform not found.
	myGLCall(glUniform1f(u_ID, v0));
}

void MyGLShader::setGLlUniformMatrix4fv(const GLchar* uniformName, const GLfloat* value, GLsizei count, GLboolean transpose)
{
	myGLCall(auto u_ID = glGetUniformLocation(this->shaderProgramID, uniformName));
	// ASSERT(u_ID != -1);  // Uniform not found.
	myGLCall(glUniformMatrix4fv(u_ID, count, transpose, value));
}

void MyGLShader::setGLlUniformMat4f(const GLchar* uniformName, const glm::mat4& mat, GLsizei count, GLboolean transpose)
{
	setGLlUniformMatrix4fv(uniformName, &mat[0][0], count, transpose);
}

void MyGLShader::setGLlUniform1i(const GLchar* uniformName, const GLint value)
{
	myGLCall(auto u_ID = glGetUniformLocation(this->shaderProgramID, uniformName));
	// ASSERT(u_ID != -1);  // Uniform not found.
	myGLCall(glUniform1i(u_ID, value));
}

void MyGLShader::setGLlUniform1ui(const GLchar* uniformName, const GLuint value)
{
	myGLCall(auto u_ID = glGetUniformLocation(this->shaderProgramID, uniformName));
	ASSERT(u_ID != -1);  // Uniform not found.
	myGLCall(glUniform1ui(u_ID, value));
}