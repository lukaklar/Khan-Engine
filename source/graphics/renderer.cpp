#include "graphics/precomp.h"
#include "graphics/renderer.hpp"
#include "graphics/hal/buffer.hpp"
#include "graphics/hal/renderbackend.hpp"
#include "graphics/hal/renderdevice.hpp"
#include "graphics/hal/swapchain.hpp"
#include "system/window.hpp"

namespace Khan
{
	Renderer::Renderer()
		: m_ThreadPool(1)
		, m_TiledDeferredDispatchParams(2 * sizeof(glm::vec4))
		, m_ScreenToViewParams(sizeof(glm::mat4) + sizeof(glm::vec2))
		, m_ScreenDimensionsChanged(true)
	{
	}

	Renderer::~Renderer()
	{
		DestroyScreenFrustumBuffer();
	}

	void Renderer::PreRender()
	{
		m_ResourceBoard.m_Persistent.m_FinalOutput = RenderBackend::g_Swapchain->GetCurrentBackBuffer();
		
		SchedulePasses();
	}

	void Renderer::Render()
	{
		RenderBackend::g_Device->GetRenderGraph().Execute(*this);
	}

	void Renderer::PostRender()
	{
		RenderBackend::g_Swapchain->Flip();
		m_OpaqueMeshes.clear();
	}

	inline void Renderer::SchedulePasses()
	{
		RenderGraph& rg = RenderBackend::g_Device->GetRenderGraph();

		/*if (m_ScreenDimensionsChanged)
		{
			RecreateScreenFrustumBuffer();
			rg.AddPass(m_TileFrustumCalculationPass);
			m_ScreenSizeChanged = false;
		}

		rg.AddPass(m_DepthPrePass);
		rg.AddPass(m_GBufferPass);
		rg.AddPass(m_LightDataUploadPass);
		rg.AddPass(m_LightCullingPass);
		rg.AddPass(m_TiledDeferredLightingPass);*/
		//rg.AddPass(m_TransparentPass);
		//rg.AddPass(m_HDRPass);
		rg.AddPass(m_TestPass);
		rg.AddPass(m_FinalPass);

		rg.Setup(*this);
		rg.Compile();
	}

	inline void Renderer::RecreateScreenFrustumBuffer()
	{
		DestroyScreenFrustumBuffer();

		BufferDesc desc;
		desc.m_Size = 4 * 4 * sizeof(float) * Window::g_Width / K_TILE_SIZE * Window::g_Height / K_TILE_SIZE;
		desc.m_Flags = BufferFlag_AllowUnorderedAccess | BufferFlag_AllowShaderResource;
		m_ResourceBoard.m_Persistent.m_ScreenFrustums = RenderBackend::g_Device->CreateBuffer(desc);
	}

	inline void Renderer::DestroyScreenFrustumBuffer()
	{
		if (m_ResourceBoard.m_Persistent.m_ScreenFrustums)
		{
			RenderBackend::g_Device->DestroyBuffer(m_ResourceBoard.m_Persistent.m_ScreenFrustums);
		}
	}
}