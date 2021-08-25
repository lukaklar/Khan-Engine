#pragma once
#include <cstdint>
#include <vector>

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
		std::vector<Texture*> m_BackBuffers;
		uint32_t m_CurrentImageIndex = 0;
		uint32_t m_FrameIndex = 0;
	};
}