#include "graphics/precomp.h"
#include "graphics/renderer.hpp"
#include "graphicshal/buffer.hpp"
#include "graphicshal/renderbackend.hpp"
#include "graphicshal/renderdevice.hpp"
#include "graphicshal/swapchain.hpp"
#include "core/defines.h"

namespace Khan
{
	Renderer::Renderer()
		: m_ThreadPool(1)
		, m_FrustumParams(sizeof(FrustumParams))
		, m_FrustumParamsChanged(true)
	{
		BufferDesc desc;

		desc.m_Size = sizeof(float);
		desc.m_Flags = BufferFlag_AllowShaderResource | BufferFlag_AllowUnorderedAccess;
		m_ResourceBoard.m_Persistent.m_AdaptedLuminance = RenderBackend::g_Device->CreateBuffer(desc);
	}

	Renderer::~Renderer()
	{
		RenderBackend::g_Device->DestroyBuffer(m_ResourceBoard.m_Persistent.m_AdaptedLuminance);
		DestroyClustersBuffer();
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

		if (m_FrustumParamsChanged)
		{
			FrustumParams frustumParams;

			frustumParams.m_Projection = m_ActiveCamera->GetProjection();
			frustumParams.m_InverseProjection = m_ActiveCamera->GetInverseProjection();
			frustumParams.m_ScreenDimensions = { m_ActiveCamera->GetViewportWidth(), m_ActiveCamera->GetViewportHeight() };
			frustumParams.m_Near = m_ActiveCamera->GetNearClip();
			frustumParams.m_Far = m_ActiveCamera->GetFarClip();
			frustumParams.m_ClusterCount = { glm::ceil((float)m_ActiveCamera->GetViewportWidth() / K_TILE_SIZE), glm::ceil((float)m_ActiveCamera->GetViewportHeight() / K_TILE_SIZE), K_NUM_DEPTH_SLICES };
			frustumParams.m_TileSize = K_TILE_SIZE;

			m_FrustumParams.UpdateConstantData(&frustumParams, 0, sizeof(FrustumParams));

			m_TotalNumClusters = frustumParams.m_ClusterCount.x * frustumParams.m_ClusterCount.y * frustumParams.m_ClusterCount.z;

			RecreateClustersBuffer();
			rg.AddPass(m_ClusterCalculationPass);

			m_FrustumParamsChanged = false;
		}

		rg.AddPass(m_DepthPrePass);
		rg.AddPass(m_GBufferPass);
		rg.AddPass(m_LightDataUploadPass);
		rg.AddPass(m_MarkActiveClustersPass);
		rg.AddPass(m_CompactActiveClustersPass);
		rg.AddPass(m_LightCullingPass);
		rg.AddPass(m_SSAOPass);
		rg.AddPass(m_DeferredLightingPass);
		//rg.AddPass(m_TransparentPass);
		rg.AddPass(m_AdaptationPass);
		rg.AddPass(m_TonemappingPass);
		rg.AddPass(m_FXAAPass);
		rg.AddPass(m_FinalPass);

		rg.Setup(*this);
		rg.Compile();
	}

	inline void Renderer::RecreateClustersBuffer()
	{
		DestroyClustersBuffer();

		BufferDesc desc;
		desc.m_Size = 3 * 4 * sizeof(float) * m_TotalNumClusters;
		desc.m_Flags = BufferFlag_AllowUnorderedAccess | BufferFlag_AllowShaderResource;
		m_ResourceBoard.m_Persistent.m_Clusters = RenderBackend::g_Device->CreateBuffer(desc);
		KH_DEBUGONLY(m_ResourceBoard.m_Persistent.m_Clusters->SetDebugName("Frustum Clusters"));
	}

	inline void Renderer::DestroyClustersBuffer()
	{
		if (m_ResourceBoard.m_Persistent.m_Clusters)
		{
			RenderBackend::g_Device->DestroyBuffer(m_ResourceBoard.m_Persistent.m_Clusters);
		}
	}
}