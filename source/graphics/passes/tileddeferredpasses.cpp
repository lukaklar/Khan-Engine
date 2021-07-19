#include "graphics/precomp.h"
#include "graphics/passes/tileddeferredpasses.hpp"
#include "graphics/hal/pixelformats.hpp"
#include "graphics/hal/queuetype.hpp"
#include "graphics/hal/renderbackend.hpp"
#include "graphics/hal/shadermanager.hpp"
#include "graphics/hal/texture.hpp"
#include "graphics/hal/textureview.hpp"
#include "graphics/renderer.hpp"
#include "graphics/rendergraph.hpp"
#include "core/defines.h"

#ifdef KH_GFXAPI_VULKAN
#include "graphics/hal/vulkan/vulkancontext.hpp"
#include "graphics/hal/vulkan/vulkandevice.hpp"
#endif // KH_GFXAPI_VULKAN

#define DECLARE_GBUFFER_INPUT(target, state)\
temp = renderer.GetResourceBoard().m_Transient.m_GBuffer.m_##target;\
viewDesc.m_Format = temp->GetDesc().m_Format;\
m_GBuffer_##target = renderGraph.DeclareResourceDependency(temp, viewDesc, state);

namespace Khan
{
	LightDataUploadPass::LightDataUploadPass()
		: RenderPass(QueueType_Copy, "LightDataUploadPass")
	{
	}

	void LightDataUploadPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		BufferDesc desc;
		desc.m_Size = static_cast<uint32_t>(renderer.GetActiveLightData().size());
		desc.m_Flags = BufferFlag_AllowUnorderedAccess | BufferFlag_AllowShaderResource;

		Buffer* temp = renderGraph.CreateManagedResource(desc);
		renderer.GetResourceBoard().m_Transient.m_ActiveSceneLights = temp;

		BufferViewDesc viewDesc;
		viewDesc.m_Offset = 0;
		viewDesc.m_Range = desc.m_Size;
		viewDesc.m_Format = PF_NONE;

		m_LightData = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_CopyDestination);
	}

	void LightDataUploadPass::Execute(RenderContext& context, Renderer& renderer)
	{
		std::vector<ShaderLightData>& lights = renderer.GetActiveLightData();
		uint32_t sizeInBytes = static_cast<uint32_t>(lights.size() * sizeof(ShaderLightData));

		context.UpdateBufferFromHost(&m_LightData->GetBuffer(), lights.data(), sizeInBytes);
	}

	TileFrustumCalculationPass::TileFrustumCalculationPass()
		: RenderPass(QueueType_Compute, "TileFrustumCalculationPass")
	{
		ComputePipelineDescription desc;
		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("tilefrustumcalculation_CS", "CS_ComputeFrustums");

		m_PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void TileFrustumCalculationPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		Buffer* screenFrustums = renderer.GetResourceBoard().m_Persistent.m_ScreenFrustums;

		BufferViewDesc desc;
		desc.m_Offset = 0;
		desc.m_Range = screenFrustums->GetDesc().m_Size;
		desc.m_Format = PF_NONE;

		m_PerTileFrustums = renderGraph.DeclareResourceDependency(screenFrustums, desc, ResourceState_UnorderedAccess);
	}

	void TileFrustumCalculationPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &renderer.GetTiledDeferredDispatchParams());
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 1, &renderer.GetScreenToViewParams());

		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 0, m_PerTileFrustums);

		// TODO: context.Dispatch(...);
	}

	LightCullingPass::LightCullingPass()
		: RenderPass(QueueType_Compute, "LightCullingPass")
	{
		ComputePipelineDescription desc;
		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("tileddeferredculling_CS", "CS_CullLights");

		m_PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void LightCullingPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		{
			Buffer* temp;
			BufferDesc desc;
			BufferViewDesc viewDesc;

			desc.m_Size = sizeof(uint32_t);
			desc.m_Flags = BufferFlag_AllowUnorderedAccess;
			temp = renderGraph.CreateManagedResource(desc);

			viewDesc.m_Offset = 0;
			viewDesc.m_Range = desc.m_Size;
			viewDesc.m_Format = PF_NONE;
			m_OpaqueLightIndexCounter = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

			temp = renderGraph.CreateManagedResource(desc);
			m_TransparentLightIndexCounter = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

			temp = renderer.GetResourceBoard().m_Persistent.m_ScreenFrustums;
			viewDesc.m_Range = temp->GetDesc().m_Size;
			m_PerTileFrustums = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

			temp = renderer.GetResourceBoard().m_Transient.m_ActiveSceneLights;
			viewDesc.m_Range = temp->GetDesc().m_Size;
			m_Lights = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

			desc.m_Size = 10000;
			desc.m_Flags = BufferFlag_AllowUnorderedAccess | BufferFlag_AllowShaderResource;
			temp = renderGraph.CreateManagedResource(desc);
			renderer.GetResourceBoard().m_Transient.m_OpaqueLightIndexList = temp;

			viewDesc.m_Range = desc.m_Size;
			m_OpaqueLightIndexList = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

			temp = renderGraph.CreateManagedResource(desc);
			renderer.GetResourceBoard().m_Transient.m_TransparentLightIndexList = temp;

			viewDesc.m_Range = desc.m_Size;
			m_TransparentLightIndexList = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);
		}

		{
			Texture* temp;
			TextureDesc desc;
			TextureViewDesc viewDesc;

			desc.m_Type = TextureType_2D;
			desc.m_Width = 1280 / 16;
			desc.m_Height = 720 / 16;
			desc.m_Depth = 1;
			desc.m_ArrayLayers = 1;
			desc.m_MipLevels = 1;		
			desc.m_Format = PF_R32G32_UINT;
			desc.m_Flags = TextureFlag_AllowUnorderedAccess | TextureFlag_AllowShaderResource;

			temp = renderGraph.CreateManagedResource(desc);
			renderer.GetResourceBoard().m_Transient.m_OpaqueLightGrid = temp;

			viewDesc.m_Type = TextureViewType_2D;
			viewDesc.m_Format = PF_R32G32_UINT;
			viewDesc.m_BaseArrayLayer = 0;
			viewDesc.m_LayerCount = 1;
			viewDesc.m_BaseMipLevel = 0;
			viewDesc.m_LevelCount = 1;

			m_OpaqueLightGrid = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

			temp = renderGraph.CreateManagedResource(desc);
			renderer.GetResourceBoard().m_Transient.m_TransparentLightGrid = temp;

			m_TransparentLightGrid = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

			temp = renderer.GetResourceBoard().m_Transient.m_GBuffer.m_Depth;
			viewDesc.m_Format = temp->GetDesc().m_Format;
			m_DepthTexture = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);
		}
	}

	void LightCullingPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &renderer.GetTiledDeferredDispatchParams());
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 1, &renderer.GetScreenToViewParams());

		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_DepthTexture);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 1, m_PerTileFrustums);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 2, m_Lights);

		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 0, m_OpaqueLightIndexCounter);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 1, m_TransparentLightIndexCounter);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 2, m_OpaqueLightIndexList);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 3, m_TransparentLightIndexList);
		context.SetUAVTexture(ResourceBindFrequency_PerFrame, 4, m_OpaqueLightGrid);
		context.SetUAVTexture(ResourceBindFrequency_PerFrame, 5, m_TransparentLightGrid);

		// TODO: context.Dispatch(...);
	}

	TiledDeferredLightingPass::TiledDeferredLightingPass()
		: RenderPass(QueueType_Compute, "TiledDeferredLightingPass")
	{
		ComputePipelineDescription desc;
		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("tileddeferredlighting_CS", "CS_TiledDeferredLighting");

		m_PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void TiledDeferredLightingPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		{
			Buffer* temp = renderer.GetResourceBoard().m_Transient.m_ActiveSceneLights;

			BufferViewDesc viewDesc;
			viewDesc.m_Offset = 0;
			viewDesc.m_Range = temp->GetDesc().m_Size;
			viewDesc.m_Format = PF_NONE;
			m_LightData = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

			// TODO: light grid and indices
			temp = renderer.GetResourceBoard().m_Transient.m_OpaqueLightIndexList;
			viewDesc.m_Range = temp->GetDesc().m_Size;
			m_LightIndexList = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);
		}

		{
			Texture* temp;

			TextureDesc desc;
			desc.m_Type = TextureType_2D;
			desc.m_Width = 1280;
			desc.m_Height = 720;
			desc.m_Depth = 1;
			desc.m_ArrayLayers = 1;
			desc.m_MipLevels = 1;
			//desc.m_SampleCount;
			desc.m_Format = PF_R16G16B16A16_FLOAT;
			desc.m_Flags = TextureFlag_AllowShaderResource | TextureFlag_AllowRenderTarget | TextureFlag_AllowUnorderedAccess;

			temp = renderGraph.CreateManagedResource(desc);
			KH_DEBUGONLY(temp->SetDebugName("LightAccumulationBuffer"));
			renderer.GetResourceBoard().m_Transient.m_LightAccumulationBuffer = temp;

			TextureViewDesc viewDesc;
			viewDesc.m_Type = TextureViewType_2D;
			viewDesc.m_BaseArrayLayer = 0;
			viewDesc.m_LayerCount = 1;
			viewDesc.m_BaseMipLevel = 0;
			viewDesc.m_LevelCount = 1;

			DECLARE_GBUFFER_INPUT(Albedo, ResourceState_NonPixelShaderAccess);
			DECLARE_GBUFFER_INPUT(Normals, ResourceState_NonPixelShaderAccess);
			DECLARE_GBUFFER_INPUT(Emissive, ResourceState_NonPixelShaderAccess);
			DECLARE_GBUFFER_INPUT(SpecularReflectance, ResourceState_NonPixelShaderAccess);
			DECLARE_GBUFFER_INPUT(MetallicAndRoughness, ResourceState_NonPixelShaderAccess);
			DECLARE_GBUFFER_INPUT(MotionVectors, ResourceState_NonPixelShaderAccess);
			DECLARE_GBUFFER_INPUT(Depth, ResourceState_NonPixelShaderAccess);

			temp = renderer.GetResourceBoard().m_Transient.m_OpaqueLightGrid;
			viewDesc.m_Format = temp->GetDesc().m_Format;
			m_LightGrid = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

			// TODO: Will need a shadow map

			viewDesc.m_Format = PF_R16G16B16A16_FLOAT;
			m_LightingResult = renderGraph.DeclareResourceDependency(renderer.GetResourceBoard().m_Transient.m_LightAccumulationBuffer, viewDesc, ResourceState_UnorderedAccess);
		}
	}

	void TiledDeferredLightingPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &renderer.GetTiledDeferredDispatchParams());
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 1, &renderer.GetScreenToViewParams());

		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_GBuffer_Albedo);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 1, m_GBuffer_Normals);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 2, m_GBuffer_SpecularReflectance);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 3, m_GBuffer_MetallicAndRoughness);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 4, m_GBuffer_Depth);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 5, m_LightIndexList);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 6, m_LightGrid);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 7, m_LightData);

		// TODO: these should be equal to the screen dimensions (get them from the active camera in renderer)
		//context.Dispatch(width, height, 1);
	}
}