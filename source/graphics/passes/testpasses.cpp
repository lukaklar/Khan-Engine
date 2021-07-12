#include "graphics/precomp.h"
#include "graphics/passes/testpasses.hpp"
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
	TestPass::TestPass()
		: RenderPass(QueueType_Graphics, "TestPass")
	{
		{
			PhysicalRenderPassDescription desc;
			desc.m_RenderTargetCount = 1;
			desc.m_RenderTargets[0].m_Format = RenderBackend::g_Swapchain->GetCurrentBackBuffer()->GetDesc().m_Format;
			desc.m_RenderTargets[0].m_StartAccess = StartAccessType::Clear;
			desc.m_RenderTargets[0].m_EndAccess = EndAccessType::Keep;

			m_PhysicalRenderPass = RenderBackend::g_Device->CreatePhysicalRenderPass(desc);
		}

		{
			GraphicsPipelineDescription desc;
			desc.m_VertexShader = ShaderManager::Get().GetShader<ShaderType_Vertex>("test_VS", "VS_Main");
			desc.m_PixelShader = ShaderManager::Get().GetShader<ShaderType_Pixel>("test_PS", "PS_Main");
			desc.m_PhysicalRenderPass = m_PhysicalRenderPass;

			m_PipelineState = RenderBackend::g_Device->CreateGraphicsPipelineState(desc);
		}
	}

	void TestPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		TextureViewDesc viewDesc;
		viewDesc.m_Type = TextureViewType_2D;
		viewDesc.m_Format = renderer.GetResourceBlackboard().m_Persistent.m_FinalOutput->GetDesc().m_Format;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;

		m_FinalOutput = renderGraph.DeclareResourceDependency(renderer.GetResourceBlackboard().m_Persistent.m_FinalOutput, viewDesc, ResourceState_RenderTarget);
	}

	void TestPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.BeginPhysicalRenderPass(*m_PhysicalRenderPass, &m_FinalOutput, nullptr);
		context.SetPipelineState(*m_PipelineState);
		context.SetViewport(0.0f, 0.0f, (float)m_FinalOutput->GetTexture().GetDesc().m_Width, (float)m_FinalOutput->GetTexture().GetDesc().m_Height);
		context.SetScissor(0, 0, m_FinalOutput->GetTexture().GetDesc().m_Width, m_FinalOutput->GetTexture().GetDesc().m_Height);
		context.DrawInstanced(3, 1, 0, 0);
		context.EndPhysicalRenderPass();
	}
}