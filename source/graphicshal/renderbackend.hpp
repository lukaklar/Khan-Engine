#pragma once
#include "core/containers.h"

namespace Khan
{
	class Display;
	class DisplayAdapter;
	//class RenderContext;
	class RenderDevice;
	class Swapchain;

	namespace RenderBackend
	{
		extern TArray<DisplayAdapter*> g_Adapters;
		extern RenderDevice* g_Device;
		//extern TArray<RenderContext*> g_Contexts;
		extern Display* g_Display;
		extern Swapchain* g_Swapchain;

		void Initialize(bool enableDebugMode);
		void Shutdown();
	}
}