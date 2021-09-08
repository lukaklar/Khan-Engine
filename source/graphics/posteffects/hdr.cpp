#include "graphics/precomp.h"
#include "graphics/posteffects/hdr.hpp"
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
#include "core/defines.h"

namespace Khan
{
	/*HDRPass::HDRPass()
		: RenderPass(QueueType_Compute, "HDRPass")
		, m_DownScaleConstants(4 * sizeof(uint32_t))
		, m_TonemapConstants(2 * sizeof(float))
	{
		{
			ComputePipelineDescription desc;

			desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("DownScalePass1_CS", "CS_DownScalePass1");
			m_DownScalePass1PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);

			desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("DownScalePass2_CS", "CS_DownScalePass2");
			m_DownScalePass2PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);

			desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("TonemapPass_CS", "CS_TonemapPass");
			m_TonemapPassPipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
		}
	}

	void HDRPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		renderGraph.EnableAsyncCompute(true);

		{
			Buffer* temp;
			BufferDesc desc;
			BufferViewDesc viewDesc;

			desc.m_Size = sizeof(float) * 1280 * 720 / (16 * 1024);
			desc.m_Flags = BufferFlag_AllowShaderResource | BufferFlag_AllowUnorderedAccess;
			temp = renderGraph.CreateManagedResource(desc);

			viewDesc.m_Offset = 0;
			viewDesc.m_Range = desc.m_Size;
			viewDesc.m_Format = PF_NONE;
			m_IntermediateLuminanceValues = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

			desc.m_Size = sizeof(float);
			temp = renderGraph.CreateManagedResource(desc);

			viewDesc.m_Range = desc.m_Size;
			m_AverageLuminanceValue = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);
		}

		{
			TextureViewDesc viewDesc;
			viewDesc.m_Type = TextureViewType_2D;
			viewDesc.m_BaseArrayLayer = 0;
			viewDesc.m_LayerCount = 1;
			viewDesc.m_BaseMipLevel = 0;
			viewDesc.m_LevelCount = 1;

			viewDesc.m_Format = renderer.GetResourceBoard().m_Transient.m_LightAccumulationBuffer->GetDesc().m_Format;
			m_LightAccumulationBuffer = renderGraph.DeclareResourceDependency(renderer.GetResourceBoard().m_Transient.m_LightAccumulationBuffer, viewDesc, ResourceState_NonPixelShaderAccess);

			viewDesc.m_Format = renderer.GetResourceBoard().m_Persistent.m_FinalOutput->GetDesc().m_Format;
			m_HDROutput = renderGraph.DeclareResourceDependency(renderer.GetResourceBoard().m_Persistent.m_FinalOutput, viewDesc, ResourceState_UnorderedAccess);
		}
	}

	void HDRPass::Execute(RenderContext& context, Renderer& renderer)
	{
		uint32_t threadGroupCountX = static_cast<uint32_t>(glm::ceil(1280.0f * 720.0f / (16.0f * 1024.0f)));
		uint32_t downScaleConsts[4] = { 1280, 720, 1280 * 720 / 16, threadGroupCountX };
		m_DownScaleConstants.UpdateConstantData(downScaleConsts, 0, sizeof(downScaleConsts));

		context.SetPipelineState(*m_DownScalePass1PipelineState);

		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &m_DownScaleConstants);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_LightAccumulationBuffer);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 0, m_IntermediateLuminanceValues);
		
		context.Dispatch(threadGroupCountX, 1, 1);

		context.SetPipelineState(*m_DownScalePass2PipelineState);

		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 0, m_IntermediateLuminanceValues);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 0, m_AverageLuminanceValue);

		context.Dispatch(1, 1, 1);

		float tonemapConsts[2] = { 0.5, 1 };
		m_TonemapConstants.UpdateConstantData(tonemapConsts, 0, sizeof(tonemapConsts));

		context.SetPipelineState(*m_TonemapPassPipelineState);

		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_LightAccumulationBuffer);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 1, m_AverageLuminanceValue);
		context.SetUAVTexture(ResourceBindFrequency_PerFrame, 0, m_HDROutput);

		uint32_t threadGroupsX = static_cast<uint32_t>(glm::ceil(1280.0f / 32.0f));
		uint32_t threadGroupsY = static_cast<uint32_t>(glm::ceil(720.0f / 32.0f));

		context.Dispatch(threadGroupsX, threadGroupsY, 1);
	}*/

	HDRPass::HDRPass()
		: RenderPass(QueueType_Compute, "HDRPass")
	{
		ComputePipelineDescription desc;
		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("hdr_CS", "CS_Tonemapping");
		m_PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void HDRPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		renderGraph.EnableAsyncCompute(true);

		Texture* temp;

		TextureViewDesc viewDesc;
		viewDesc.m_Type = TextureViewType_2D;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;

		temp = renderer.GetResourceBoard().m_Transient.m_LightAccumulationBuffer;
		viewDesc.m_Format = temp->GetDesc().m_Format;
		m_LightAccumulationBuffer = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

		temp = renderer.GetResourceBoard().m_Persistent.m_FinalOutput;
		viewDesc.m_Format = temp->GetDesc().m_Format;
		temp = renderGraph.CreateManagedResource(temp->GetDesc());
		KH_DEBUGONLY(temp->SetDebugName("Temp PostFX"));
		renderer.GetResourceBoard().m_Transient.m_TempPostFxSurface = temp;
		m_HDROutput = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

		renderer.GetResourceBoard().SwapPostFXSurfaces();
	}

	void HDRPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.SetPipelineState(*m_PipelineState);

		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_LightAccumulationBuffer);
		context.SetUAVTexture(ResourceBindFrequency_PerFrame, 0, m_HDROutput);

		const glm::uvec3& threadGroupCount = renderer.GetNumDispatchThreads();
		context.Dispatch(threadGroupCount.x, threadGroupCount.y, threadGroupCount.z);
	}
}