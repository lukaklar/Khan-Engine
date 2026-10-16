#pragma once
#include "core/primitivetypes.h"
#include "core/containers.h"

namespace Khan
{
	class Display;
	class RenderDevice;
	class Texture;

	class Swapchain
	{
	public:
		virtual ~Swapchain() {}

		virtual void Flip() = 0;
		inline Texture* GetCurrentBackBuffer() const { return m_BackBuffers[m_CurrentImageIndex]; }

	protected:
		TArray<Texture*> m_BackBuffers;
		uint32 m_CurrentImageIndex = 0;
		uint32 m_FrameIndex = 0;
	};
}