#pragma once

#ifdef VK_VALIDATE
#define VK_ASSERT(call, message)\
do\
{\
	if (call != VK_SUCCESS)\
	{\
		MessageBox(NULL, message, "Error!", MB_ICONERROR);\
		__debugbreak();\
		exit(-1);\
	}\
} while (0)
#else
#define VK_ASSERT(call, message) do { call; } while (0)
#endif // VK_VALIDATE