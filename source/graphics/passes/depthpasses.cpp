#include "graphics/precomp.h"
#include "graphics/passes/depthpasses.hpp"
#include "graphics/hal/pixelformats.hpp"
#include "graphics/hal/queuetype.hpp"
#include "graphics/hal/texture.hpp"
#include "graphics/hal/textureview.hpp"
#include "graphics/hal/renderbackend.hpp"
#include "graphics/renderer.hpp"
#include "graphics/rendergraph.hpp"
#include "graphics/hal/pipelinedescriptions.hpp"
#include "graphics/hal/shadermanager.hpp"
#include "graphics/objects/mesh.hpp"
#include "graphics/materials/material.hpp"

#ifdef KH_GFXAPI_VULKAN
#include "graphics/hal/vulkan/vulkancontext.hpp"
#include "graphics/hal/vulkan/vulkandevice.hpp"
#include "graphics/hal/vulkan/vulkanswapchain.hpp"
#endif // KH_GFXAPI_VULKAN

namespace Khan
{
	DepthPrePass::DepthPrePass()
		: RenderPass(QueueType_Graphics, "DepthPrePass")
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
			desc.m_VertexShader = ShaderManager::Get().GetShader<ShaderType_Vertex>("C:\\dev\\Khan\\source\\graphics\\shaders\\vert.spv", "main");
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::POSITION);
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float2, VertexInputState::StreamDescriptor::StreamElement::Usage::TEXCOORD0);
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::NORMAL);
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::BITANGENT);
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::TANGENT);
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
		desc.m_Width = 1280;
		desc.m_Height = 720;
		desc.m_Depth = 1;
		desc.m_ArrayLayers = 1;
		desc.m_MipLevels = 1;
		//desc.m_SampleCount;
		desc.m_Flags = TextureFlag_AllowShaderResource | TextureFlag_AllowDepthStencil;

		Texture* depthBuffer = renderGraph.CreateManagedResource(desc);
		renderer.GetResourceBlackboard().m_Transient.m_GBuffer.m_Depth = depthBuffer;

		TextureViewDesc viewDesc;
		viewDesc.m_Type = TextureViewType_2D;
		viewDesc.m_Format = renderer.GetResourceBlackboard().m_Persistent.m_FinalOutput->GetDesc().m_Format;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;

		m_DepthBuffer = renderGraph.DeclareResourceDependency(depthBuffer, viewDesc, ResourceState_DepthWriteStencilWrite);
	}

	void DepthPrePass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.BeginPhysicalRenderPass(*m_PhysicalRenderPass, nullptr, m_DepthBuffer);
		
		auto& meshes = renderer.GetOpaqueMeshes();
		for (auto* mesh : meshes)
		{
			context.SetVertexBuffer(0, mesh->GetVertexBuffer(), 0);
			context.SetIndexBuffer(mesh->GetIndexBuffer(), 0, false);

			auto& submeshes = mesh->GetSubMeshData();
			for (auto& submesh : submeshes)
			{
				Material* material = submesh.m_Material;

				// TODO: Get material and check if it has both faces and in accordance with that use the appropriate pipeline state
				RenderPipelineState* pipelineState = material->HasTwoSides() ? m_PipelineStateNoCulling : m_PipelineStateBackfaceCulling;

				context.SetPipelineState(*pipelineState);
				context.SetViewport(0.0f, 0.0f, (float)m_DepthBuffer->GetTexture().GetDesc().m_Width, (float)m_DepthBuffer->GetTexture().GetDesc().m_Height);
				context.SetScissor(0, 0, m_DepthBuffer->GetTexture().GetDesc().m_Width, m_DepthBuffer->GetTexture().GetDesc().m_Height);
				context.DrawIndexedInstanced(submesh.m_NumIndices, 1, submesh.m_IndexBufferOffset, submesh.m_VertexBufferOffset, 0);
			}
		}

		context.EndPhysicalRenderPass();
	}
}