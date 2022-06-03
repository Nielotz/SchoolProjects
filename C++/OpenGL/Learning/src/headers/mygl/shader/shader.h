#pragma once
#include "../../color.h"
#include "../../drawable/primitive.h"

#include <glad/glad.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class MyGLShader
{
public:
	GLuint getShaderProgramID();

	GLuint createProgramFromFile(const std::string& path);

	void setGLUniform1f(const GLchar* uniformName, GLfloat v0);
	void setGLlUniform1i(const GLchar* uniformName, const GLint value);
	void setGLlUniform1ui(const GLchar* uniformName, const GLuint value);

	void setGLUniform3f(const GLchar* uniformName, GLfloat v0, GLfloat v1, GLfloat v2);
	void setGLUniform3f(const GLchar* uniformName, drawable::primitive::Point3D);
	void setGLUniform3f(const GLchar* uniformName, color::RGB color);
	void setGLUniform3f(const GLchar* uniformName, glm::vec3 vec);

	void setGLUniform4f(const GLchar* uniformName, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	void setGLUniform4f(const GLchar* uniformName, color::RGBA color);
	void setGLUniform4f(const GLchar* uniformName, color::RGB color);

	void setGLlUniformMatrix4fv(const GLchar* uniformName, const GLfloat* value, GLsizei count = 1, GLboolean transpose = GL_FALSE);
	void setGLlUniformMat4f(const GLchar* uniformName, const glm::mat4& mat, GLsizei count = 1, GLboolean transpose = GL_FALSE);

private:
	GLuint shaderProgramID;
	const std::string path;

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
};
