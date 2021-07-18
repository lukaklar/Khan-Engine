#pragma once

#ifdef KH_DEBUG
#define KH_ASSERT(expression, message)\
do {\
	if (!(expression))\
	{\
		MessageBox(NULL, message, "Error!", MB_ICONERROR);\
		__debugbreak();\
		exit(-1);\
	}\
} while (0)
#else
#define KH_ASSERT(expression, message)
#endif // KH_DEBUG