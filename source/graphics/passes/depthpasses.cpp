#include "graphics/precomp.h"
#include "graphics/passes/depthpasses.hpp"
#include "graphicshal/physicalrenderpass.hpp"
#include "graphicshal/pipelinedescriptions.hpp"
#include "graphicshal/pixelformats.hpp"
#include "graphicshal/queuetype.hpp"
#include "graphicshal/renderbackend.hpp"
#include "graphicshal/rendercontext.hpp"
#include "graphicshal/resourcebindfrequency.hpp"
#include "graphicshal/texture.hpp"
#include "graphicshal/textureview.hpp"
#include "graphics/materials/material.hpp"
#include "graphics/objects/mesh.hpp"
#include "graphics/renderer.hpp"
#include "graphics/rendergraph.hpp"
#include "graphics/shadermanager.hpp"
#include "core/defines.h"

namespace Khan
{
	DepthPrePass::DepthPrePass()
		: RenderPass(QueueType_Graphics, "DepthPrePass")
		, m_ViewProjParams(sizeof(glm::mat4))
	{
		{
			PhysicalRenderPassDescription desc;
			desc.m_DepthStencil.m_Format = PF_D32_FLOAT;
			desc.m_DepthStencil.m_DepthStartAccess = StartAccessType::Clear;
			desc.m_DepthStencil.m_DepthEndAccess = EndAccessType::Keep;

			m_PhysicalRenderPass = RenderBackend::g_Device->CreatePhysicalRenderPass(desc);
		}

		{
			GraphicsPipelineDescription desc;
			desc.m_VertexShader = ShaderManager::Get()->GetShader<ShaderType_Vertex>("depth_VS", "VS_Main");
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::POSITION);
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float2, VertexInputState::StreamDescriptor::StreamElement::Usage::TEXCOORD0);
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::NORMAL);
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::TANGENT);
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::BITANGENT);
			desc.m_DepthStencilState.m_DepthMode.m_DepthTestEnabled = true;
			desc.m_DepthStencilState.m_DepthMode.m_DepthWriteEnabled = true;
			desc.m_DepthStencilState.m_DepthMode.m_DepthFunc = DepthStencilState::CompareFunction::Less;
			desc.m_RasterizerState.m_CullMode = RasterizerState::CullMode::None;
			desc.m_PhysicalRenderPass = m_PhysicalRenderPass;

			m_PipelineStateNoCulling = RenderBackend::g_Device->CreateGraphicsPipelineState(desc);

			desc.m_RasterizerState.m_CullMode = RasterizerState::CullMode::Back;
			m_PipelineStateBackfaceCulling = RenderBackend::g_Device->CreateGraphicsPipelineState(desc);
		}
	}

	void DepthPrePass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		TextureDesc desc;
		desc.m_Type = TextureType_2D;
		desc.m_Width = renderer.GetActiveCamera()->GetViewportWidth();
		desc.m_Height = renderer.GetActiveCamera()->GetViewportHeight();
		desc.m_Depth = 1;
		desc.m_ArrayLayers = 1;
		desc.m_MipLevels = 1;
		desc.m_Format = PF_D32_FLOAT;
		desc.m_Flags = TextureFlag_AllowShaderResource | TextureFlag_AllowDepthStencil;

		Texture* temp = renderGraph.CreateManagedResource(desc);
		KH_DEBUGONLY(temp->SetDebugName("Depth Texture"));
		renderer.GetResourceBoard().m_Transient.m_GBuffer.m_Depth = temp;

		TextureViewDesc viewDesc;
		viewDesc.m_Type = TextureViewType_2D;
		viewDesc.m_Format = desc.m_Format;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;

		m_DepthTexture = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_DepthWriteStencilWrite);
	}

	void DepthPrePass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.BeginPhysicalRenderPass(*m_PhysicalRenderPass, nullptr, m_DepthTexture);

		m_ViewProjParams.UpdateConstantData(&renderer.GetActiveCamera()->GetViewProjection(), 0, sizeof(glm::mat4));
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &m_ViewProjParams);

		for (auto mesh : renderer.GetOpaqueMeshes())
		{
			Material* material = mesh->m_Material;

			RenderPipelineState* pipelineState = material->HasTwoSides() ? m_PipelineStateNoCulling : m_PipelineStateBackfaceCulling;

			context.SetPipelineState(*pipelineState);
			context.SetViewport(0.0f, 0.0f, (float)renderer.GetActiveCamera()->GetViewportWidth(), (float)renderer.GetActiveCamera()->GetViewportHeight());
			context.SetScissor(0, 0, renderer.GetActiveCamera()->GetViewportWidth(), renderer.GetActiveCamera()->GetViewportHeight());

			context.SetVertexBuffer(0, mesh->m_VertexBuffer, 0);
			context.SetIndexBuffer(mesh->m_IndexBuffer, 0, false);

			context.SetConstantBuffer(ResourceBindFrequency_PerDraw, 0, &mesh->m_ParentTransform);

			context.DrawIndexedInstanced(mesh->m_IndexCount, 1, 0, 0, 0);
		}

		context.EndPhysicalRenderPass();
	}
}