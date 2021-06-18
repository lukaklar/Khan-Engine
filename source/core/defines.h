#pragma once

#ifdef KH_DEBUG
	#define KH_DEBUGONLY(expression) expression
#else
	#define KH_DEBUGONLY(expression)
#endif