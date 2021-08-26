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
		inline Renderer& GetRenderer() const { return *m_Renderer; }

	private:
		GraphicsManager();
		~GraphicsManager();

		Renderer* m_Renderer;
	};
}