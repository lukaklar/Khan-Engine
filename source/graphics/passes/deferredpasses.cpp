#include "graphics/precomp.h"
#include "graphics/passes/deferredpasses.hpp"
#include "graphics/hal/buffer.hpp"
#include "graphics/hal/bufferview.hpp"
#include "graphics/hal/pixelformats.hpp"
#include "graphics/hal/queuetype.hpp"
#include "graphics/hal/renderbackend.hpp"
#include "graphics/hal/rendercontext.hpp"
#include "graphics/hal/resourcebindfrequency.hpp"
#include "graphics/hal/texture.hpp"
#include "graphics/hal/textureview.hpp"
#include "graphics/renderer.hpp"
#include "graphics/rendergraph.hpp"
#include "graphics/shadermanager.hpp"
#include "core/defines.h"

#define DECLARE_GBUFFER_INPUT(target, state)\
temp = renderer.GetResourceBoard().m_Transient.m_GBuffer.m_##target;\
viewDesc.m_Format = temp->GetDesc().m_Format;\
m_GBuffer_##target = renderGraph.DeclareResourceDependency(temp, viewDesc, state);

namespace Khan
{
	ClusterDeferredLightingPass::ClusterDeferredLightingPass()
		: RenderPass(QueueType_Compute, "ClusterDeferredLightingPass")
	{
		ComputePipelineDescription desc;
		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("clusterdeferredlighting_CS", "CS_DeferredLighting");

		m_PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void ClusterDeferredLightingPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		renderGraph.EnableAsyncCompute(true);

		{
			Buffer* temp;
			BufferViewDesc viewDesc;

			temp = renderer.GetResourceBoard().m_Transient.m_ActiveSceneLights;
			viewDesc.m_Offset = 0;
			viewDesc.m_Range = temp->GetDesc().m_Size;
			viewDesc.m_Format = PF_NONE;
			m_LightData = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

			// TODO: light grid and indices
			temp = renderer.GetResourceBoard().m_Transient.m_LightIndexList;
			viewDesc.m_Range = temp->GetDesc().m_Size;
			viewDesc.m_Format = PF_R32_UINT;
			m_LightIndexList = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

			temp = renderer.GetResourceBoard().m_Transient.m_LightGrid;
			viewDesc.m_Range = temp->GetDesc().m_Size;
			viewDesc.m_Format = PF_R32G32_UINT;
			m_LightGrid = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);
		}

		{
			Texture* temp;

			TextureDesc desc;
			desc.m_Type = TextureType_2D;
			desc.m_Width = renderer.GetActiveCamera()->GetViewportWidth();
			desc.m_Height = renderer.GetActiveCamera()->GetViewportHeight();
			desc.m_Depth = 1;
			desc.m_ArrayLayers = 1;
			desc.m_MipLevels = 1;
			desc.m_Format = PF_R16G16B16A16_FLOAT;
			desc.m_Flags = TextureFlag_AllowShaderResource | TextureFlag_AllowUnorderedAccess;

			temp = renderGraph.CreateManagedResource(desc);
			KH_DEBUGONLY(temp->SetDebugName("LightAccumulationBuffer"));
			renderer.GetResourceBoard().m_Transient.m_LightAccumulationBuffer = temp;
			//temp = renderer.GetResourceBoard().m_Persistent.m_FinalOutput;

			TextureViewDesc viewDesc;
			viewDesc.m_Type = TextureViewType_2D;
			viewDesc.m_BaseArrayLayer = 0;
			viewDesc.m_LayerCount = 1;
			viewDesc.m_BaseMipLevel = 0;
			viewDesc.m_LevelCount = 1;

			//viewDesc.m_Format = desc.m_Format;
			viewDesc.m_Format = temp->GetDesc().m_Format;
			m_LightingResult = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

			DECLARE_GBUFFER_INPUT(Albedo, ResourceState_NonPixelShaderAccess);
			DECLARE_GBUFFER_INPUT(Normals, ResourceState_NonPixelShaderAccess);
			DECLARE_GBUFFER_INPUT(Emissive, ResourceState_NonPixelShaderAccess);
			DECLARE_GBUFFER_INPUT(PBRConsts, ResourceState_NonPixelShaderAccess);
			DECLARE_GBUFFER_INPUT(Depth, ResourceState_NonPixelShaderAccess);

			temp = renderer.GetResourceBoard().m_Transient.m_AmbientOcclusionFactors;
			viewDesc.m_Format = temp->GetDesc().m_Format;
			m_AOTexture = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

			// TODO: Will need a shadow map
		}
	}

	void ClusterDeferredLightingPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.SetPipelineState(*m_PipelineState);
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &renderer.GetFrustumParams());
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_GBuffer_Albedo);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 1, m_GBuffer_Normals);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 2, m_GBuffer_PBRConsts);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 3, m_GBuffer_Depth);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 4, m_AOTexture);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 5, m_LightData);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 6, m_LightIndexList);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 7, m_LightGrid);
		context.SetUAVTexture(ResourceBindFrequency_PerFrame, 0, m_LightingResult);

		uint32_t threadGroupCountX = (uint32_t)glm::ceil((float)renderer.GetActiveCamera()->GetViewportWidth() / 16);
		uint32_t threadGroupCountY = (uint32_t)glm::ceil((float)renderer.GetActiveCamera()->GetViewportHeight() / 16);
		context.Dispatch(threadGroupCountX, threadGroupCountY, 1);
	}

	DeferredLightingPass::DeferredLightingPass()
		: RenderPass(QueueType_Compute, "DeferredLightingPass")
		, m_LightParams(sizeof(uint32_t))
	{
		ComputePipelineDescription desc;
		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("deferredlighting_CS", "CS_DeferredLighting");

		m_PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void DeferredLightingPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		renderGraph.EnableAsyncCompute(true);

		{
			Buffer* temp = renderer.GetResourceBoard().m_Transient.m_ActiveSceneLights;

			BufferViewDesc viewDesc;
			viewDesc.m_Offset = 0;
			viewDesc.m_Range = temp->GetDesc().m_Size;
			viewDesc.m_Format = PF_NONE;
			m_LightData = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);
		}

		{
			Texture* temp;

			TextureDesc desc;
			desc.m_Type = TextureType_2D;
			desc.m_Width = renderer.GetActiveCamera()->GetViewportWidth();
			desc.m_Height = renderer.GetActiveCamera()->GetViewportHeight();
			desc.m_Depth = 1;
			desc.m_ArrayLayers = 1;
			desc.m_MipLevels = 1;
			desc.m_Format = PF_R16G16B16A16_FLOAT;
			desc.m_Flags = TextureFlag_AllowShaderResource | TextureFlag_AllowUnorderedAccess;

			temp = renderGraph.CreateManagedResource(desc);
			KH_DEBUGONLY(temp->SetDebugName("LightAccumulationBuffer"));
			renderer.GetResourceBoard().m_Transient.m_LightAccumulationBuffer = temp;
			//temp = renderer.GetResourceBoard().m_Persistent.m_FinalOutput;

			TextureViewDesc viewDesc;
			viewDesc.m_Type = TextureViewType_2D;
			viewDesc.m_BaseArrayLayer = 0;
			viewDesc.m_LayerCount = 1;
			viewDesc.m_BaseMipLevel = 0;
			viewDesc.m_LevelCount = 1;

			viewDesc.m_Format = temp->GetDesc().m_Format;
			m_LightingResult = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

			temp = renderer.GetResourceBoard().m_Transient.m_AmbientOcclusionFactors;
			viewDesc.m_Format = temp->GetDesc().m_Format;
			m_AOTexture = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

			DECLARE_GBUFFER_INPUT(Albedo, ResourceState_NonPixelShaderAccess);
			DECLARE_GBUFFER_INPUT(Normals, ResourceState_NonPixelShaderAccess);
			DECLARE_GBUFFER_INPUT(Emissive, ResourceState_NonPixelShaderAccess);
			DECLARE_GBUFFER_INPUT(PBRConsts, ResourceState_NonPixelShaderAccess);
			DECLARE_GBUFFER_INPUT(Depth, ResourceState_NonPixelShaderAccess);

			// TODO: Will need a shadow map
		}
	}

	void DeferredLightingPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.SetPipelineState(*m_PipelineState);

		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &renderer.GetFrustumParams());
		uint32_t numLights = static_cast<uint32_t>(renderer.GetActiveLightData().size());
		m_LightParams.UpdateConstantData(&numLights, 0, sizeof(uint32_t));
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 1, &m_LightParams);

		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_GBuffer_Albedo);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 1, m_GBuffer_Normals);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 2, m_GBuffer_PBRConsts);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 3, m_GBuffer_Depth);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 4, m_AOTexture);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 5, m_LightData);

		context.SetUAVTexture(ResourceBindFrequency_PerFrame, 0, m_LightingResult);

		uint32_t threadGroupCountX = (uint32_t)glm::ceil((float)renderer.GetActiveCamera()->GetViewportWidth() / 16);
		uint32_t threadGroupCountY = (uint32_t)glm::ceil((float)renderer.GetActiveCamera()->GetViewportHeight() / 16);
		context.Dispatch(threadGroupCountX, threadGroupCountY, 1);
	}
}