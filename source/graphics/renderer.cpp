#include "graphics/precomp.h"
#include "graphics/renderer.hpp"
#include "graphics/hal/renderbackend.hpp"

#ifdef KH_GFXAPI_VULKAN
#include "graphics/hal/vulkan/vulkandevice.hpp"
#include "graphics/hal/vulkan/vulkanswapchain.hpp"
#endif // KH_GFXAPI_VULKAN

#include "system/window.hpp"


namespace Khan
{
	Renderer::Renderer()
		: m_ThreadPool(1)
		, m_ScreenSizeChanged(true)
	{
	}

	void Renderer::PreRender()
	{
		m_ResourceBlackboard.m_FinalOutput = RenderBackend::g_Swapchain->GetCurrentBackBuffer();
		// TODO: Possibly also perform frustum culling and node processing in general in a separate thread to pass scheduling and rendergraph compilation
		SchedulePasses();
	}

	void Renderer::Render()
	{
		RenderBackend::g_Device->GetRenderGraph().Execute(*this);
	}

	void Renderer::PostRender()
	{
		RenderBackend::g_Swapchain->Flip();
	}

	inline void Renderer::SchedulePasses()
	{
		RenderGraph& rg = RenderBackend::g_Device->GetRenderGraph();

		if (m_ScreenSizeChanged)
		{
			RecreateScreenFrustumBuffer();
			rg.AddPass(m_TileFrustumCalculationPass);
			m_ScreenSizeChanged = false;
		}

		rg.AddPass(m_DepthPrePass);
		rg.AddPass(m_LightCullingPass);
		rg.AddPass(m_GBufferPass);
		rg.AddPass(m_TiledDeferredLightingPass);
		//rg.AddPass(m_TransparentPass);
		//rg.AddPass(m_HDRPass);
		rg.AddPass(m_TestPass);
		rg.AddPass(m_FinalPass);
		rg.Setup(*this);
		rg.Compile();
	}

	inline void Renderer::RecreateScreenFrustumBuffer()
	{
		if (m_ResourceBlackboard.m_ScreenFrustums)
		{
			RenderBackend::g_Device->DestroyBuffer(m_ResourceBlackboard.m_ScreenFrustums);
		}

		BufferDesc desc;
		desc.m_Size = 4 * 4 * sizeof(float) * Window::g_Width / K_TILE_SIZE * Window::g_Height / K_TILE_SIZE;
		desc.m_Flags = BufferFlag_AllowUnorderedAccess | BufferFlag_AllowShaderResource;

		m_ResourceBlackboard.m_ScreenFrustums = RenderBackend::g_Device->CreateBuffer(desc);
	}
}