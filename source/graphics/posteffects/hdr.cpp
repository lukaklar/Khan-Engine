#include "graphics/precomp.h"
#include "graphics/posteffects/hdr.hpp"
#include "graphics/hal/pixelformats.hpp"
#include "graphics/hal/queuetype.hpp"
#include "graphics/hal/texture.hpp"
#include "graphics/hal/textureview.hpp"
#include "graphics/hal/renderbackend.hpp"
#include "graphics/renderer.hpp"
#include "graphics/rendergraph.hpp"
#include "graphics/hal/pipelinedescriptions.hpp"
#include "graphics/hal/shadermanager.hpp"

#ifdef KH_GFXAPI_VULKAN
#include "graphics/hal/vulkan/vulkancontext.hpp"
#include "graphics/hal/vulkan/vulkandevice.hpp"
#include "graphics/hal/vulkan/vulkanswapchain.hpp"
#endif // KH_GFXAPI_VULKAN

namespace Khan
{
	HDRPass::HDRPass()
		: RenderPass(QueueType_Compute, "HDRPass")
	{
		ComputePipelineDescription desc;

		//m_PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void HDRPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		renderGraph.EnableAsyncCompute(true);

		TextureViewDesc viewDesc;
		viewDesc.m_Type = TextureViewType_2D;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;

		viewDesc.m_Format = renderer.GetResourceBoard().m_Transient.m_LightAccumulationBuffer->GetDesc().m_Format;
		m_LightAccumulationBuffer = renderGraph.DeclareResourceDependency(renderer.GetResourceBoard().m_Transient.m_LightAccumulationBuffer, viewDesc, ResourceState_NonPixelShaderAccess);

		viewDesc.m_Format = renderer.GetResourceBoard().m_Persistent.m_FinalOutput->GetDesc().m_Format;
		m_FinalOutput = renderGraph.DeclareResourceDependency(renderer.GetResourceBoard().m_Persistent.m_FinalOutput, viewDesc, ResourceState_UnorderedAccess);
	}

	void HDRPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.SetPipelineState(*m_PipelineState);
	}
}