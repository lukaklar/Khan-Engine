#pragma once
#include "core/singleton.h"

namespace Khan
{
	class Renderer;

	class GraphicsManager : public Singleton<GraphicsManager>
	{
		friend class Singleton<GraphicsManager>;
	public:
		void Render();

	private:
		GraphicsManager();
		~GraphicsManager();

		Renderer* m_Renderer;
	};
}