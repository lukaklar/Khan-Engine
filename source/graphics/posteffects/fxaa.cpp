#include "graphics/precomp.h"
#include "graphics/posteffects/fxaa.hpp"
#include "graphicshal/buffer.hpp"
#include "graphicshal/bufferview.hpp"
#include "graphicshal/physicalrenderpass.hpp"
#include "graphicshal/pipelinedescriptions.hpp"
#include "graphicshal/pixelformats.hpp"
#include "graphicshal/queuetype.hpp"
#include "graphicshal/renderbackend.hpp"
#include "graphicshal/rendercontext.hpp"
#include "graphicshal/resourcebindfrequency.hpp"
#include "graphicshal/texture.hpp"
#include "graphicshal/textureview.hpp"
#include "graphics/renderer.hpp"
#include "graphics/rendergraph.hpp"
#include "graphics/shadermanager.hpp"

namespace Khan
{
	FXAAPass::FXAAPass()
		: RenderPass(QueueType_Compute, "FXAAPass")
	{
		ComputePipelineDescription desc;
		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("fxaa_filter_CS", "CS_FXAAFilter");
		m_PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void FXAAPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		renderGraph.EnableAsyncCompute(true);

		TextureViewDesc viewDesc;
		viewDesc.m_Type = TextureViewType_2D;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;
		viewDesc.m_Format = renderer.GetResourceBoard().m_Persistent.m_FinalOutput->GetDesc().m_Format;

		m_InputTexture = renderGraph.DeclareResourceDependency(renderer.GetResourceBoard().GetPostFXSrc(), viewDesc, ResourceState_NonPixelShaderAccess);
		m_OutputTexture = renderGraph.DeclareResourceDependency(renderer.GetResourceBoard().GetPostFXDst(), viewDesc, ResourceState_UnorderedAccess);

		renderer.GetResourceBoard().SwapPostFXSurfaces();
	}

	void FXAAPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.SetPipelineState(*m_PipelineState);
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &renderer.GetFrustumParams());
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_InputTexture);
		context.SetUAVTexture(ResourceBindFrequency_PerFrame, 0, m_OutputTexture);

		uint32_t threadGroupCountX = (uint32_t)glm::ceil((float)renderer.GetActiveCamera()->GetViewportWidth() / 16);
		uint32_t threadGroupCountY = (uint32_t)glm::ceil((float)renderer.GetActiveCamera()->GetViewportHeight() / 16);
		context.Dispatch(threadGroupCountX, threadGroupCountY, 1);
	}
}