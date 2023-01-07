#include "graphics/precomp.h"
#include "graphics/passes/finalpasses.hpp"
#include "graphicshal/pixelformats.hpp"
#include "graphicshal/queuetype.hpp"
#include "graphicshal/renderbackend.hpp"
#include "graphicshal/rendercontext.hpp"
#include "graphicshal/swapchain.hpp"
#include "graphicshal/texture.hpp"
#include "graphicshal/textureview.hpp"
#include "graphics/renderer.hpp"
#include "graphics/rendergraph.hpp"
#include "graphicshal/pipelinedescriptions.hpp"
#include "graphics/shadermanager.hpp"

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
		viewDesc.m_Format = renderer.GetResourceBoard().m_Persistent.m_FinalOutput->GetDesc().m_Format;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;

		if (renderer.GetResourceBoard().m_Persistent.m_FinalOutput != RenderBackend::g_Swapchain->GetCurrentBackBuffer())
		{
			m_InputTexture = renderGraph.DeclareResourceDependency(renderer.GetResourceBoard().GetPostFXSrc(), viewDesc, ResourceState_CopySource);
			m_FinalOutput = renderGraph.DeclareResourceDependency(renderer.GetResourceBoard().GetPostFXDst(), viewDesc, ResourceState_Undefined, ResourceState_Present);
		}
		else
		{
			m_FinalOutput = renderGraph.DeclareResourceDependency(renderer.GetResourceBoard().m_Persistent.m_FinalOutput, viewDesc, ResourceState_Present);
		}
	}

	void FinalPass::Execute(RenderContext& context, Renderer& renderer)
	{
		if (renderer.GetResourceBoard().m_Persistent.m_FinalOutput != RenderBackend::g_Swapchain->GetCurrentBackBuffer())
		{
			context.CopyTexture(m_InputTexture, { 0, 0, 0 }, m_FinalOutput, { 0, 0, 0 }, { m_FinalOutput->GetTexture().GetDesc().m_Width, m_FinalOutput->GetTexture().GetDesc().m_Height, 1 });
		}
	}
}