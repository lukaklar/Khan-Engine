#pragma once

namespace Khan
{
	class Display;
	class DisplayAdapter;
	//class RenderContext;
	class RenderDevice;
	class Swapchain;

	namespace RenderBackend
	{
		extern std::vector<DisplayAdapter*> g_Adapters;
		extern RenderDevice* g_Device;
		//extern std::vector<RenderContext*> g_Contexts;
		extern Display* g_Display;
		extern Swapchain* g_Swapchain;

		void Initialize(bool enableDebugMode);
		void Shutdown();
		void AdvanceFrame();
	}
}