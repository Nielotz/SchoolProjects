#pragma once

#include <glad/glad.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class MyGLShader
{
	GLuint shaderProgramID;
	enum class ShaderType : int
	{
		None = -1,
		Vertex = 0,
		Fragment = 1
	};

	static const std::vector<std::string> ShaderTypeName;

	std::string checkShaderStatus(const GLuint& id, const GLenum& operationType);

	std::string checkProgramStatus(const GLuint& id, const GLenum& operationType);

	GLuint compile(const ShaderType& shaderType, const std::string& sourceCode);

	std::unordered_map<ShaderType, std::string> parseFromFile(const char* filename);

public:
	GLuint createProgram();
	GLuint getShaderProgramID();
	void setGLUniform4f(const GLchar* uniformName, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	void setGLlUniformMatrix4fv(const GLchar* uniformName, const GLfloat* value, GLsizei count = 1, GLboolean transpose = GL_FALSE);
	void setGLlUniformMat4f(const GLchar* uniformName, const glm::mat4& mat, GLsizei count = 1, GLboolean transpose = GL_FALSE);
	void setGLlUniform1i(const GLchar* uniformName, const GLint value);
	
};
