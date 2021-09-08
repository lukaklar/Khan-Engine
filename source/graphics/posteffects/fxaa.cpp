#include "graphics/precomp.h"
#include "graphics/posteffects/fxaa.hpp"
#include "graphics/hal/buffer.hpp"
#include "graphics/hal/bufferview.hpp"
#include "graphics/hal/physicalrenderpass.hpp"
#include "graphics/hal/pipelinedescriptions.hpp"
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

namespace Khan
{
	FXAAPass::FXAAPass()
		: RenderPass(QueueType_Compute, "FXAAPass")
		, m_ScreenDimensions(sizeof(glm::vec2))
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

		glm::vec2 dim(renderer.GetActiveCamera()->GetViewportWidth(), renderer.GetActiveCamera()->GetViewportHeight());
		m_ScreenDimensions.UpdateConstantData(&dim, 0, sizeof(glm::vec2));
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &m_ScreenDimensions);

		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_InputTexture);
		context.SetUAVTexture(ResourceBindFrequency_PerFrame, 0, m_OutputTexture);

		const glm::uvec3& threadGroupCount = renderer.GetNumDispatchThreads();
		context.Dispatch(threadGroupCount.x, threadGroupCount.y, threadGroupCount.z);
	}
}