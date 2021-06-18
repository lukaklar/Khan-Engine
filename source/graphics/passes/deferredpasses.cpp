#include "graphics/precomp.h"
#include "graphics/passes/deferredpasses.hpp"
#include "graphics/hal/pixelformats.hpp"
#include "graphics/hal/queuetype.hpp"
#include "graphics/hal/texture.hpp"
#include "graphics/hal/textureview.hpp"
#include "graphics/hal/renderbackend.hpp"
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
	DeferredLightingPass::DeferredLightingPass()
		: RenderPass(QueueType_Graphics, "DeferredLightingPass")
	{
		PhysicalRenderPassDescription desc;
		desc.m_RenderTargetCount = 1;
		desc.m_RenderTargets[0].m_Format = PF_R16G16B16A16_FLOAT;
		desc.m_RenderTargets[0].m_StartAccess = StartAccessType::Clear;
		desc.m_RenderTargets[0].m_EndAccess = EndAccessType::Keep;
		desc.m_DepthStencil.m_Format = PF_D32_FLOAT;
		desc.m_DepthStencil.m_DepthStartAccess = StartAccessType::Keep;
		desc.m_DepthStencil.m_DepthEndAccess = EndAccessType::Keep;

		m_PhysicalRenderPass = RenderBackend::g_Device->CreatePhysicalRenderPass(desc);
	}

	void DeferredLightingPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
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
		desc.m_Flags = TextureFlag_AllowShaderResource | TextureFlag_AllowRenderTarget;

		temp = renderGraph.CreateManagedResource(desc);
		KH_DEBUGONLY(temp->SetDebugName("LightAccumulationBuffer"));
		renderer.GetResourceBlackboard().m_LightAccumulationBuffer = temp;

		TextureViewDesc viewDesc;
		viewDesc.m_Type = TextureViewType_2D;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;

		DECLARE_GBUFFER_INPUT(Albedo, ResourceState_PixelShaderAccess);
		DECLARE_GBUFFER_INPUT(Normals, ResourceState_PixelShaderAccess);
		DECLARE_GBUFFER_INPUT(Emissive, ResourceState_PixelShaderAccess);
		DECLARE_GBUFFER_INPUT(SpecularReflectance, ResourceState_PixelShaderAccess);
		DECLARE_GBUFFER_INPUT(MetallicAndRoughness, ResourceState_PixelShaderAccess);
		DECLARE_GBUFFER_INPUT(MotionVectors, ResourceState_PixelShaderAccess);
		DECLARE_GBUFFER_INPUT(Depth, ResourceState_DepthWriteStencilWrite);

		// TODO: Will need a shadow map

		viewDesc.m_Format = PF_R16G16B16A16_FLOAT;
		m_LightAccumulationBuffer = renderGraph.DeclareResourceDependency(renderer.GetResourceBlackboard().m_LightAccumulationBuffer, viewDesc, ResourceState_RenderTarget);
	}

	void DeferredLightingPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.BeginPhysicalRenderPass(*m_PhysicalRenderPass, &m_LightAccumulationBuffer, m_GBuffer_Depth);

		context.EndPhysicalRenderPass();
	}
}