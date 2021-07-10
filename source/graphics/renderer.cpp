#include "graphics/precomp.h"
#include "graphics/renderer.hpp"
#include "graphics/hal/renderbackend.hpp"

#ifdef KH_GFXAPI_VULKAN
#include "graphics/hal/vulkan/vulkandevice.hpp"
#include "graphics/hal/vulkan/vulkanswapchain.hpp"
#endif // KH_GFXAPI_VULKAN


namespace Khan
{
	Renderer::Renderer()
		: m_ThreadPool(1)
		//, m_DispatchParams(2 * sizeof(glm::uvec4))
		//, m_ScreenToViewParams(sizeof(glm::mat4) + sizeof(glm::vec2))
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
		//rg.AddPass(m_DepthPrePass);
		//rg.AddPass(m_GBufferPass);
		//rg.AddPass(m_TiledDeferredLightingPass);
		//rg.AddPass(m_TransparentPass);
		//rg.AddPass(m_HDRPass);
		rg.AddPass(m_TestPass);
		rg.AddPass(m_FinalPass);
		rg.Setup(*this);
		rg.Compile();
	}
}