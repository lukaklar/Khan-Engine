#include "graphics/precomp.h"
#include "graphics/passes/finalpasses.hpp"
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
	FinalPass::FinalPass()
		: RenderPass(QueueType_Graphics, "FinalPass")
	{
	}

	void FinalPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		TextureViewDesc viewDesc;
		viewDesc.m_Type = TextureViewType_2D;
		viewDesc.m_Format = renderer.GetResourceBlackboard().m_FinalOutput->GetDesc().m_Format;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;

		m_FinalOutput = renderGraph.DeclareResourceDependency(renderer.GetResourceBlackboard().m_FinalOutput, viewDesc, ResourceState_Present);
	}

	void FinalPass::Execute(RenderContext& context, Renderer& renderer)
	{
	}
}