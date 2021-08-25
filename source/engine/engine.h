#pragma once
#include "graphics/renderer.hpp"

namespace Khan
{
	class World;

	namespace Engine
	{
		void Run();

		extern volatile bool g_Running;

		extern Renderer* g_Renderer;
	}
}