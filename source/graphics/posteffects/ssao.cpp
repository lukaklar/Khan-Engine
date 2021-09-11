#include "graphics/precomp.h"
#include "graphics/posteffects/ssao.hpp"
#include "graphics/hal/physicalrenderpass.hpp"
#include "graphics/hal/pipelinedescriptions.hpp"
#include "graphics/hal/pixelformats.hpp"
#include "graphics/hal/queuetype.hpp"
#include "graphics/hal/renderbackend.hpp"
#include "graphics/hal/rendercontext.hpp"
#include "graphics/hal/resourcebindfrequency.hpp"
#include "graphics/hal/texture.hpp"
#include "graphics/hal/textureview.hpp"
#include "graphics/materials/material.hpp"
#include "graphics/objects/mesh.hpp"
#include "graphics/renderer.hpp"
#include "graphics/rendergraph.hpp"
#include "graphics/shadermanager.hpp"
#include "core/defines.h"

namespace Khan
{
	SSAOPass::SSAOPass()
		: RenderPass(QueueType_Compute, "SSAOPass")
		, m_SampleParams((64 + 16) * sizeof(glm::vec4))
		, m_ProjectionParams(sizeof(glm::mat4))
	{
		auto lerp = [](float a, float b, float f) -> float
		{
			return a + f * (b - a);
		};

		std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
		std::default_random_engine generator;

		glm::vec4 ssaoKernel[64];
		for (unsigned int i = 0; i < 64; ++i)
		{
			glm::vec4 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator), 0.0f);
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			float scale = i / 64.0f;

			// scale samples s.t. they're more aligned to center of kernel
			scale = lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;
			ssaoKernel[i] = sample;
		}

		glm::vec4 ssaoNoise[16];
		for (unsigned int i = 0; i < 16; i++)
		{
			ssaoNoise[i] = glm::vec4(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f, 0.0f);
		}

		m_SampleParams.UpdateConstantData(ssaoKernel, 0, 64 * sizeof(glm::vec4));
		m_SampleParams.UpdateConstantData(ssaoNoise, 64 * sizeof(glm::vec4), 16 * sizeof(glm::vec4));

		ComputePipelineDescription desc;

		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("ssao_calculate_CS", "CS_SSAOCalculate");
		m_CalculationPipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);

		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("ssao_blur_CS", "CS_SSAOBlur");
		m_BlurPipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void SSAOPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		renderGraph.EnableAsyncCompute(true);

		Texture* temp;

		TextureDesc desc;
		desc.m_Type = TextureType_2D;
		desc.m_Width = renderer.GetActiveCamera()->GetViewportWidth();
		desc.m_Height = renderer.GetActiveCamera()->GetViewportHeight();
		desc.m_Depth = 1;
		desc.m_ArrayLayers = 1;
		desc.m_MipLevels = 1;
		desc.m_Format = PF_R32_FLOAT;
		desc.m_Flags = TextureFlag_AllowUnorderedAccess | TextureFlag_AllowShaderResource;

		TextureViewDesc viewDesc;
		viewDesc.m_Type = TextureViewType_2D;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;

		temp = renderGraph.CreateManagedResource(desc);
		KH_DEBUGONLY(temp->SetDebugName("SSAO"));
		viewDesc.m_Format = desc.m_Format;
		m_SSAOTexture = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

		temp = renderGraph.CreateManagedResource(desc);
		KH_DEBUGONLY(temp->SetDebugName("Blurred SSAO"));
		renderer.GetResourceBoard().m_Transient.m_AmbientOcclusionFactors = temp;
		viewDesc.m_Format = desc.m_Format;
		m_SSAOBlurTexture = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

		temp = renderer.GetResourceBoard().m_Transient.m_GBuffer.m_Normals;
		viewDesc.m_Format = temp->GetDesc().m_Format;
		m_GBuffer_Normals = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

		temp = renderer.GetResourceBoard().m_Transient.m_GBuffer.m_Depth;
		viewDesc.m_Format = temp->GetDesc().m_Format;
		m_GBuffer_Depth = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);
	}

	void SSAOPass::Execute(RenderContext& context, Renderer& renderer)
	{
		const glm::uvec3& threadGroupCount = renderer.GetNumDispatchThreads();

		context.SetPipelineState(*m_CalculationPipelineState);

		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &m_SampleParams);
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 1, &renderer.GetScreenToViewParams());
		m_ProjectionParams.UpdateConstantData(&renderer.GetActiveCamera()->GetProjection(), 0, sizeof(glm::mat4));
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 2, &m_ProjectionParams);

		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_GBuffer_Normals);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 1, m_GBuffer_Depth);

		context.SetUAVTexture(ResourceBindFrequency_PerFrame, 0, m_SSAOTexture);

		context.Dispatch(threadGroupCount.x, threadGroupCount.y, threadGroupCount.z);

		context.SetPipelineState(*m_BlurPipelineState);

		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, nullptr);
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 2, nullptr);

		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 0, m_SSAOTexture);
		context.SetSRVTexture(ResourceBindFrequency_PerFrame, 1, nullptr);

		context.SetUAVTexture(ResourceBindFrequency_PerFrame, 0, m_SSAOBlurTexture);

		context.Dispatch(threadGroupCount.x, threadGroupCount.y, threadGroupCount.z);
	}
}