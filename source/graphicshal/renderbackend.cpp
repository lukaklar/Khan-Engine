#include "graphicshal/precomp.h"
#include "graphicshal/renderbackend.hpp"

namespace Khan
{
	namespace RenderBackend
	{
		std::vector<DisplayAdapter*> g_Adapters;
		RenderDevice* g_Device;
		//std::vector<RenderContext*> g_Contexts;
		Display* g_Display;
		Swapchain* g_Swapchain;
	}
}