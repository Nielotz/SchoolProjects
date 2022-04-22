#pragma once

#include <glad/glad.h>
#include <iostream>
#include <vector>

class MyGLShader
{
	enum class ShaderType : int
	{
		None = -1,
		Vertex = 0,
		Fragment = 1
	};

	static const std::vector<std::string> ShaderTypeName;

	static std::string checkShaderStatus(const GLuint& id, const GLenum& operationType);

	static std::string checkProgramStatus(const GLuint& id, const GLenum& operationType);

	static GLuint compile(const ShaderType& shaderType, const std::string& sourceCode);

	static std::unordered_map<ShaderType, std::string> parseFromFile(const char* filename);

public:
	static GLuint createProgram();

};
