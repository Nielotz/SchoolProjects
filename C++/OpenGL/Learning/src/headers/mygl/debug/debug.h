#pragma once
#include "../../logging/logging.h"

#define ASSERT(x) if(!(x)) __debugbreak();
#define ASSERTC(x, callOnFalse) if(!(x)) callOnFalse();
#define myGLCall(x) ASSERT(myGLShowCallErrors(true, #x, __FILE__, __LINE__));\
					x;\
					ASSERT(myGLShowCallErrors(false, #x, __FILE__, __LINE__))

bool myGLShowCallErrors(bool beforeFunctionCall, const char* function, const char* file, int line);
