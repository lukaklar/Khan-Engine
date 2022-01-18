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
	LuminanceAdaptationPass::LuminanceAdaptationPass()
		: RenderPass(QueueType_Compute, "LuminanceAdaptationPass")
		, m_LuminanceHistogramBuffer(16)
		, m_LuminanceHistogramAverageBuffer(20)
	{
		ComputePipelineDescription desc;

		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("computehistogram_CS", "CS_ComputeHistogram");
		m_ComputeHistogramPipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);

		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("averagehistogram_CS", "CS_AverageHistogram");
		m_AverageHistogramPipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void LuminanceAdaptationPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		renderGraph.EnableAsyncCompute(true);

		{
			Buffer* temp;
			BufferDesc desc;
			BufferViewDesc viewDesc;

			desc.m_Size = K_NUM_HISTOGRAM_BINS * sizeof(uint32_t);
			desc.m_Flags = BufferFlag_AllowShaderResource | BufferFlag_AllowUnorderedAccess | BufferFlag_Writable;
			temp = renderGraph.CreateManagedResource(desc);

			viewDesc.m_Offset = 0;
			viewDesc.m_Range = temp->GetDesc().m_Size;
			viewDesc.m_Format = PF_R32_UINT;
			m_Histogram = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_CopyDestination);


			temp = renderer.GetResourceBoard().m_Persistent.m_AdaptedLuminance;
			viewDesc.m_Offset = 0;
			viewDesc.m_Range = temp->GetDesc().m_Size;
			viewDesc.m_Format = PF_R32_FLOAT;
			m_Luminance = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);
		}

		{
			Texture* temp;

			TextureViewDesc viewDesc;
			viewDesc.m_Type = TextureViewType_2D;
			viewDesc.m_BaseArrayLayer = 0;
			viewDesc.m_LayerCount = 1;
			viewDesc.m_BaseMipLevel = 0;
			viewDesc.m_LevelCount = 1;

			temp = renderer.GetResourceBoard().m_Transient.m_LightAccumulationBuffer;
			viewDesc.m_Format = temp->GetDesc().m_Format;
			m_HDRTexture = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);
		}
	}

	void LuminanceAdaptationPass::Execute(RenderContext& context, Renderer& renderer)
	{
		uint32_t width = renderer.GetActiveCamera()->GetViewportWidth();
		uint32_t height = renderer.GetActiveCamera()->GetViewportHeight();
		float oneOverLogLuminanceRange = 1.0f / (K_MAX_LOG_LUMINANCE - K_MIN_LOG_LUMINANCE);

		m_LuminanceHistogramBuffer.UpdateConstantData(&width, 0, 4);
		m_LuminanceHistogramBuffer.UpdateConstantData(&height, 4, 4);
		m_LuminanceHistogramBuffer.UpdateConstantData(&K_MIN_LOG_LUMINANCE, 8, 4);
		m_LuminanceHistogramBuffer.UpdateConstantData(&oneOverLogLuminanceRange, 12, 4);

		context.ClearBuffer(m_Histogram, 0);
		context.SetPipelineState(*m_ComputeHistogramPipelineState);
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &m_LuminanceHistogramBuffer);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_HDRTexture);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 0, m_Histogram);

		uint32_t threadGroupCountX = (uint32_t)glm::ceil((float)width / 16);
		uint32_t threadGroupCountY = (uint32_t)glm::ceil((float)height / 16);
		context.Dispatch(threadGroupCountX, threadGroupCountY, 1);

		uint32_t pixelCount = width * height;
		float logLuminanceRange = K_MAX_LOG_LUMINANCE - K_MIN_LOG_LUMINANCE;
		std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
		std::chrono::duration<float> elapsed_seconds = timeNow - m_LastFrameTime;
		m_LastFrameTime = timeNow;
		float deltaTime = elapsed_seconds.count();

		m_LuminanceHistogramAverageBuffer.UpdateConstantData(&pixelCount, 0, 4);
		m_LuminanceHistogramAverageBuffer.UpdateConstantData(&K_MIN_LOG_LUMINANCE, 4, 4);
		m_LuminanceHistogramAverageBuffer.UpdateConstantData(&logLuminanceRange, 8, 4);
		m_LuminanceHistogramAverageBuffer.UpdateConstantData(&deltaTime, 12, 4);
		m_LuminanceHistogramAverageBuffer.UpdateConstantData(&K_TAU, 16, 4);

		context.SetPipelineState(*m_AverageHistogramPipelineState);
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &m_LuminanceHistogramAverageBuffer);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 0, m_Histogram);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 0, m_Luminance);

		context.Dispatch(1, 1, 1);
	}

	TonemappingPass::TonemappingPass()
		: RenderPass(QueueType_Compute, "TonemappingPass")
	{
		ComputePipelineDescription desc;
		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("tonemap_CS", "CS_Tonemap");
		m_PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void TonemappingPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		renderGraph.EnableAsyncCompute(true);

		{
			Buffer* temp = renderer.GetResourceBoard().m_Persistent.m_AdaptedLuminance;

			BufferViewDesc viewDesc;
			viewDesc.m_Offset = 0;
			viewDesc.m_Range = temp->GetDesc().m_Size;
			viewDesc.m_Format = PF_R32_FLOAT;

			m_AdaptedLuminance = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);
		}

		{
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
	}

	void TonemappingPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.SetPipelineState(*m_PipelineState);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_LightAccumulationBuffer);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 1, m_AdaptedLuminance);
		context.SetUAVTexture(ResourceBindFrequency_PerFrame, 0, m_HDROutput);

		uint32_t threadGroupCountX = (uint32_t)glm::ceil((float)renderer.GetActiveCamera()->GetViewportWidth() / 16);
		uint32_t threadGroupCountY = (uint32_t)glm::ceil((float)renderer.GetActiveCamera()->GetViewportHeight() / 16);
		context.Dispatch(threadGroupCountX, threadGroupCountY, 1);
	}
}