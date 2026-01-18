#pragma once
#include "../../logging/logging.h"

// Cross-platform debug break
#ifdef _WIN32
    #define DEBUG_BREAK() __debugbreak()
#elif defined(__linux__) || defined(__APPLE__)
    #include <signal.h>
    #define DEBUG_BREAK() raise(SIGTRAP)
#else
    #include <cstdlib>
    #define DEBUG_BREAK() std::abort()
#endif

#define ASSERT(x) if(!(x)) DEBUG_BREAK();
#define ASSERTC(x, callOnFalse) if(!(x)) callOnFalse();
#define myGLCall(x) ASSERT(myGLShowCallErrors(true, #x, __FILE__, __LINE__));\
					x;\
					ASSERT(myGLShowCallErrors(false, #x, __FILE__, __LINE__))

bool myGLShowCallErrors(bool beforeFunctionCall, const char* function, const char* file, int line);
