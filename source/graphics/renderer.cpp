#include "graphics/precomp.h"
#include "graphics/renderer.hpp"
#include "graphics/hal/buffer.hpp"
#include "graphics/hal/renderbackend.hpp"
#include "graphics/hal/renderdevice.hpp"
#include "graphics/hal/swapchain.hpp"
#include "core/defines.h"

namespace Khan
{
	Renderer::Renderer()
		: m_ThreadPool(6)
		, m_TiledDeferredDispatchParams(2 * sizeof(glm::uvec4))
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
		m_ActiveLightData.clear();
	}

	inline void Renderer::SchedulePasses()
	{
		RenderGraph& rg = RenderBackend::g_Device->GetRenderGraph();

		if (m_ScreenDimensionsChanged)
		{
			RecreateScreenFrustumBuffer();
			rg.AddPass(m_TileFrustumCalculationPass);
			m_ScreenDimensionsChanged = false;

			glm::vec2 screenDimensions(m_ActiveCamera->GetViewportWidth(), m_ActiveCamera->GetViewportHeight());

			m_NumDispatchThreads.x = (uint32_t)glm::ceil(screenDimensions.x / K_TILE_SIZE);
			m_NumDispatchThreads.y = (uint32_t)glm::ceil(screenDimensions.y / K_TILE_SIZE);
			m_NumDispatchThreads.z = 1;

			m_NumDispatchThreadGroups.x = (uint32_t)glm::ceil((float)m_NumDispatchThreads.x / K_TILE_SIZE);
			m_NumDispatchThreadGroups.y = (uint32_t)glm::ceil((float)m_NumDispatchThreads.y / K_TILE_SIZE);
			m_NumDispatchThreadGroups.z = 1;

			m_TiledDeferredDispatchParams.UpdateConstantData(&m_NumDispatchThreadGroups[0], 0, sizeof(glm::uvec3));
			m_TiledDeferredDispatchParams.UpdateConstantData(&m_NumDispatchThreads[0], sizeof(glm::uvec4), sizeof(glm::uvec3));

			m_ScreenToViewParams.UpdateConstantData(&m_ActiveCamera->GetInverseProjection(), 0, sizeof(glm::mat4));
			m_ScreenToViewParams.UpdateConstantData(&screenDimensions[0], sizeof(glm::mat4), sizeof(glm::vec2));
		}

		rg.AddPass(m_DepthPrePass);
		rg.AddPass(m_GBufferPass);
		rg.AddPass(m_LightDataUploadPass);
		rg.AddPass(m_LightCullingPass);
		rg.AddPass(m_TiledDeferredLightingPass);
		//rg.AddPass(m_TransparentPass);
		//rg.AddPass(m_HDRPass);
		//rg.AddPass(m_TestPass);
		rg.AddPass(m_FinalPass);

		rg.Setup(*this);
		rg.Compile();
	}

	inline void Renderer::RecreateScreenFrustumBuffer()
	{
		DestroyScreenFrustumBuffer();

		BufferDesc desc;
		desc.m_Size = 4 * 4 * sizeof(float) * m_ActiveCamera->GetViewportWidth() / K_TILE_SIZE * m_ActiveCamera->GetViewportHeight() / K_TILE_SIZE;
		desc.m_Flags = BufferFlag_AllowUnorderedAccess | BufferFlag_AllowShaderResource;
		m_ResourceBoard.m_Persistent.m_ScreenFrustums = RenderBackend::g_Device->CreateBuffer(desc);
		KH_DEBUGONLY(m_ResourceBoard.m_Persistent.m_ScreenFrustums->SetDebugName("Screen Frustums"));
	}

	inline void Renderer::DestroyScreenFrustumBuffer()
	{
		if (m_ResourceBoard.m_Persistent.m_ScreenFrustums)
		{
			RenderBackend::g_Device->DestroyBuffer(m_ResourceBoard.m_Persistent.m_ScreenFrustums);
		}
	}
}