#include "graphicshal/precomp.h"
#include "graphicshal/renderbackend.hpp"

namespace Khan
{
	namespace RenderBackend
	{
		TArray<DisplayAdapter*> g_Adapters;
		RenderDevice* g_Device;
		//TArray<RenderContext*> g_Contexts;
		Display* g_Display;
		Swapchain* g_Swapchain;
	}
}