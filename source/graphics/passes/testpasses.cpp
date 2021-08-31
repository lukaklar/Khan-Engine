#include "graphics/precomp.h"
#include "graphics/passes/testpasses.hpp"
#include "graphics/hal/physicalrenderpass.hpp"
#include "graphics/hal/pipelinedescriptions.hpp"
#include "graphics/hal/pixelformats.hpp"
#include "graphics/hal/queuetype.hpp"
#include "graphics/hal/renderbackend.hpp"
#include "graphics/hal/rendercontext.hpp"
#include "graphics/hal/resourcebindfrequency.hpp"
#include "graphics/hal/swapchain.hpp"
#include "graphics/hal/texture.hpp"
#include "graphics/hal/textureview.hpp"
#include "graphics/renderer.hpp"
#include "graphics/rendergraph.hpp"
#include "graphics/shadermanager.hpp"
#include "graphics/objects/mesh.hpp"
#include "graphics/materials/material.hpp"

namespace Khan
{
	TestPass::TestPass()
		: RenderPass(QueueType_Graphics, "TestPass2")
		, m_PerFrameConsts(sizeof(glm::mat4))
	{
		{
			PhysicalRenderPassDescription desc;
			desc.m_RenderTargetCount = 1;
			desc.m_RenderTargets[0].m_Format = RenderBackend::g_Swapchain->GetCurrentBackBuffer()->GetDesc().m_Format;
			desc.m_RenderTargets[0].m_StartAccess = StartAccessType::Clear;
			desc.m_RenderTargets[0].m_EndAccess = EndAccessType::Keep;
			desc.m_DepthStencil.m_Format = PF_D32_FLOAT;
			desc.m_DepthStencil.m_DepthStartAccess = StartAccessType::Clear;
			desc.m_DepthStencil.m_DepthEndAccess = EndAccessType::Discard;

			m_PhysicalRenderPass = RenderBackend::g_Device->CreatePhysicalRenderPass(desc);
			m_PhysicalRenderPass->SetRenderTargetClearColor(0, 0.5f, 0.5f, 0.5f, 0.5f);
		}

		{
			m_PipelineDesc.m_VertexShader = ShaderManager::Get()->GetShader<ShaderType_Vertex>("test_VS", "VS_Test");
			m_PipelineDesc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::POSITION);
			m_PipelineDesc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::COLOR0);
			m_PipelineDesc.m_DepthStencilState.m_DepthMode.m_DepthTestEnabled = true;
			m_PipelineDesc.m_DepthStencilState.m_DepthMode.m_DepthWriteEnabled = true;
			m_PipelineDesc.m_DepthStencilState.m_DepthMode.m_DepthFunc = DepthStencilState::CompareFunction::Less;
			m_PipelineDesc.m_PhysicalRenderPass = m_PhysicalRenderPass;
		}
	}

	void TestPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		TextureViewDesc viewDesc;
		viewDesc.m_Type = TextureViewType_2D;
		viewDesc.m_Format = renderer.GetResourceBoard().m_Persistent.m_FinalOutput->GetDesc().m_Format;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;

		m_FinalOutput = renderGraph.DeclareResourceDependency(renderer.GetResourceBoard().m_Persistent.m_FinalOutput, viewDesc, ResourceState_RenderTarget);

		TextureDesc desc;
		desc.m_Type = TextureType_2D;
		desc.m_Width = 1280;
		desc.m_Height = 720;
		desc.m_Depth = 1;
		desc.m_ArrayLayers = 1;
		desc.m_MipLevels = 1;
		desc.m_Format = PF_D32_FLOAT;
		desc.m_Flags = TextureFlag_AllowDepthStencil;

		Texture* temp = renderGraph.CreateManagedResource(desc);

		viewDesc.m_Format = desc.m_Format;
		m_DepthBuffer = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_DepthWriteStencilWrite);
	}

	void TestPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.BeginPhysicalRenderPass(*m_PhysicalRenderPass, &m_FinalOutput, m_DepthBuffer);

		glm::mat4 i(1.0f);
		m_PerFrameConsts.UpdateConstantData(&renderer.GetActiveCamera()->GetViewProjection(), 0, sizeof(glm::mat4));
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &m_PerFrameConsts);

		for (Mesh* mesh : renderer.GetOpaqueMeshes())
		{
			Material* material = mesh->m_Material;

			if (!material->IsCompiled())
			{
				m_PipelineDesc.m_PixelShader = material->GetPixelShader();
				m_PipelineDesc.m_RasterizerState.m_CullMode = material->HasTwoSides() ? RasterizerState::CullMode::None : RasterizerState::CullMode::Back;

				material->SetPipelineState(context.GetDevice().CreateGraphicsPipelineState(m_PipelineDesc));
			}

			context.SetPipelineState(*material->GetPipelineState());
			context.SetViewport(0.0f, 0.0f, (float)m_FinalOutput->GetTexture().GetDesc().m_Width, (float)m_FinalOutput->GetTexture().GetDesc().m_Height);
			context.SetScissor(0, 0, m_FinalOutput->GetTexture().GetDesc().m_Width, m_FinalOutput->GetTexture().GetDesc().m_Height);

			context.SetVertexBuffer(0, mesh->m_VertexBuffer, 0);
			context.SetIndexBuffer(mesh->m_IndexBuffer, 0, false);

			context.SetConstantBuffer(ResourceBindFrequency_PerDraw, 0, &mesh->m_ParentTransform);

			context.DrawIndexedInstanced(mesh->m_IndexCount, 1, 0, 0, 0);
		}

		context.EndPhysicalRenderPass();
	}
}