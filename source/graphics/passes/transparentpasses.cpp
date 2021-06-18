#include "graphics/precomp.h"
#include "graphics/passes/transparentpasses.hpp"
#include "graphics/hal/pixelformats.hpp"
#include "graphics/hal/queuetype.hpp"
#include "graphics/hal/texture.hpp"
#include "graphics/hal/textureview.hpp"
#include "graphics/hal/renderbackend.hpp"
#include "graphics/renderer.hpp"
#include "graphics/rendergraph.hpp"

#ifdef KH_GFXAPI_VULKAN
#include "graphics/hal/vulkan/vulkancontext.hpp"
#include "graphics/hal/vulkan/vulkandevice.hpp"
#endif // KH_GFXAPI_VULKAN

namespace Khan
{
	TransparentPass::TransparentPass()
		: RenderPass(QueueType_Graphics, "TransparentPass")
	{
		PhysicalRenderPassDescription desc;
		desc.m_RenderTargetCount = 1;
		desc.m_RenderTargets[0].m_Format = PF_R16G16B16A16_FLOAT;
		desc.m_RenderTargets[0].m_StartAccess = StartAccessType::Keep;
		desc.m_RenderTargets[0].m_EndAccess = EndAccessType::Keep;
		desc.m_DepthStencil.m_Format = PF_D32_FLOAT;
		desc.m_DepthStencil.m_DepthStartAccess = StartAccessType::Keep;
		desc.m_DepthStencil.m_DepthEndAccess = EndAccessType::Keep;

		m_PhysicalRenderPass = RenderBackend::g_Device->CreatePhysicalRenderPass(desc);
	}

	void TransparentPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		TextureViewDesc viewDesc;
		viewDesc.m_Type = TextureViewType_2D;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;

		viewDesc.m_Format = renderer.GetResourceBlackboard().m_LightAccumulationBuffer->GetDesc().m_Format;
		m_ColorBuffer = renderGraph.DeclareResourceDependency(renderer.GetResourceBlackboard().m_LightAccumulationBuffer, viewDesc, ResourceState_RenderTarget, true);

		viewDesc.m_Format = PF_D32_FLOAT;
		m_DepthBuffer = renderGraph.DeclareResourceDependency(renderer.GetResourceBlackboard().GBuffer.Depth, viewDesc, ResourceState_DepthWriteStencilWrite);
	}

	void TransparentPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.BeginPhysicalRenderPass(*m_PhysicalRenderPass, &m_ColorBuffer, m_DepthBuffer);

		context.EndPhysicalRenderPass();
	}
}