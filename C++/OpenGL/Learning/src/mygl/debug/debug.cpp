#pragma once
#include <glad/glad.h>

#include <sstream>

#include "../../src/headers/mygl/debug/debug.h"
#include "../../headers/mygl/consts.h"


bool myGLShowCallErrors(bool beforeFunctionCall, const char* function, const char* file, int line)
{
	GLenum error = glGetError();
	
	if (error == GL_NO_ERROR)
		return true;

	if (beforeFunctionCall)
		logging::warning("OpenGL", "An error occured before call of this function.");

	std::stringstream errorMessageStringstream;
	errorMessageStringstream << kGLEnumIDToErrorText.at(error) << " at: " << function << " " << file << ":" << line << std::endl;
	logging::error("OpenGL", errorMessageStringstream.str());

	return false;
}
