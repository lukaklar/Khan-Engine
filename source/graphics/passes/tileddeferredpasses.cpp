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
temp = renderer.GetResourceBlackboard().GBuffer.##target;\
viewDesc.m_Format = temp->GetDesc().m_Format;\
m_GBuffer_##target = renderGraph.DeclareResourceDependency(temp, viewDesc, state);

namespace Khan
{
	TileFrustumCalculationPass::TileFrustumCalculationPass()
		: RenderPass(QueueType_Compute, "TileFrustumCalculationPass")
		, m_DispatchParams(2 * sizeof(glm::uvec4))
		, m_ScreenToViewParams(sizeof(glm::mat4) + sizeof(glm::vec2))
	{
		ComputePipelineDescription desc;
		// TODO: Set the appropriate shader and entry point
		desc.m_ComputeShader = ShaderManager::Get().GetShader<ShaderType_Vertex>("C:\\dev\\Khan\\source\\graphics\\shaders\\vert.spv", "main");

		m_PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void TileFrustumCalculationPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		Buffer* screenFrustums = renderer.GetResourceBlackboard().m_ScreenFrustums;

		BufferViewDesc desc;
		desc.m_Offset = 0;
		desc.m_Range = screenFrustums->GetDesc().m_Size;
		desc.m_Format = PF_NONE;

		m_PerTileFrustums = renderGraph.DeclareResourceDependency(screenFrustums, desc, ResourceState_UnorderedAccess);
	}

	void TileFrustumCalculationPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 0, m_PerTileFrustums);

		// TODO: Update constant buffers or better yet move them to Renderer so they can be shared between passes that use them
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &m_DispatchParams);
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 1, &m_ScreenToViewParams);

		// TODO: context.Dispatch(...);
	}

	LightCullingPass::LightCullingPass()
		: RenderPass(QueueType_Compute, "LightCullingPass")
		, m_DispatchParams(2 * sizeof(glm::uvec4))
		, m_ScreenToViewParams(sizeof(glm::mat4) + sizeof(glm::vec2))
	{

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

			temp = renderer.GetResourceBlackboard().m_ScreenFrustums;
			viewDesc.m_Range = temp->GetDesc().m_Size;
			m_PerTileFrustums = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

			temp = renderer.GetResourceBlackboard().m_SceneLights;
			viewDesc.m_Range = temp->GetDesc().m_Size;
			m_Lights = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

			desc.m_Size = 10000;
			desc.m_Flags = BufferFlag_AllowUnorderedAccess | BufferFlag_AllowShaderResource;
			temp = renderGraph.CreateManagedResource(desc);
			renderer.GetResourceBlackboard().m_OpaqueLightIndexList = temp;

			viewDesc.m_Range = desc.m_Size;
			m_OpaqueLightIndexList = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

			temp = renderGraph.CreateManagedResource(desc);
			renderer.GetResourceBlackboard().m_TransparentLightIndexList = temp;

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
			renderer.GetResourceBlackboard().m_OpaqueLightGrid = temp;

			viewDesc.m_Type = TextureViewType_2D;
			viewDesc.m_Format = PF_R32G32_UINT;
			viewDesc.m_BaseArrayLayer = 0;
			viewDesc.m_LayerCount = 1;
			viewDesc.m_BaseMipLevel = 0;
			viewDesc.m_LevelCount = 1;

			m_OpaqueLightGrid = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

			temp = renderGraph.CreateManagedResource(desc);
			renderer.GetResourceBlackboard().m_TransparentLightGrid = temp;

			m_TransparentLightGrid = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

			temp = renderer.GetResourceBlackboard().GBuffer.Depth;
			viewDesc.m_Format = temp->GetDesc().m_Format;
			m_DepthTexture = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);
		}
	}

	void LightCullingPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_DepthTexture);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 1, m_PerTileFrustums);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 2, m_Lights);

		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 0, m_OpaqueLightIndexCounter);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 1, m_TransparentLightIndexCounter);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 2, m_OpaqueLightIndexList);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 3, m_TransparentLightIndexList);
		context.SetUAVTexture(ResourceBindFrequency_PerFrame, 4, m_OpaqueLightGrid);
		context.SetUAVTexture(ResourceBindFrequency_PerFrame, 5, m_TransparentLightGrid);

		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &m_DispatchParams);
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 1, &m_ScreenToViewParams);

		// TODO: context.Dispatch(...);
	}

	TiledDeferredLightingPass::TiledDeferredLightingPass()
		: RenderPass(QueueType_Compute, "TiledDeferredLightingPass")
		, m_DispatchParams(2 * sizeof(glm::uvec4))
		, m_GBufferUnpackParams(sizeof(glm::vec4) + sizeof(glm::mat4))
	{
		ComputePipelineDescription desc;
		// TODO: Set the appropriate shader and entry point
		desc.m_ComputeShader = ShaderManager::Get().GetShader<ShaderType_Vertex>("C:\\dev\\Khan\\source\\graphics\\shaders\\vert.spv", "main");

		m_PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void TiledDeferredLightingPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
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
		renderer.GetResourceBlackboard().m_LightAccumulationBuffer = temp;

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

		// TODO: Will need a shadow map

		viewDesc.m_Format = PF_R16G16B16A16_FLOAT;
		m_LightAccumulationBuffer = renderGraph.DeclareResourceDependency(renderer.GetResourceBlackboard().m_LightAccumulationBuffer, viewDesc, ResourceState_UnorderedAccess);
	}

	void TiledDeferredLightingPass::Execute(RenderContext& context, Renderer& renderer)
	{
		uint32_t width = m_LightAccumulationBuffer->GetTexture().GetDesc().m_Width;
		uint32_t height = m_LightAccumulationBuffer->GetTexture().GetDesc().m_Height;

		glm::uvec3 numThreads(glm::ceil(width / 16), glm::ceil(height / 16), 1);
		glm::uvec3 numThreadGroups(glm::ceil(numThreads.x / 16), glm::ceil(numThreads.y / 16), 1);

		m_DispatchParams.UpdateConstantData(&numThreadGroups[0], 0, sizeof(glm::uvec3));
		m_DispatchParams.UpdateConstantData(&numThreads[0], sizeof(glm::uvec4), sizeof(glm::uvec3));

		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &m_DispatchParams);

		// TODO: Set parameters for GBuffer unpacking 1, 2

		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_GBuffer_Albedo);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 1, m_GBuffer_Normals);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 2, m_GBuffer_SpecularReflectance);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 3, m_GBuffer_MetallicAndRoughness);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 4, m_GBuffer_Depth);

		context.Dispatch(numThreads.x, numThreads.y, numThreads.z);
	}
}