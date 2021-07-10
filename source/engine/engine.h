#pragma once
#include "graphics/renderer.hpp"

namespace Khan
{
	namespace Engine
	{
		void Run();

		extern volatile bool g_Running;

		extern Renderer* g_Renderer;
	}
}