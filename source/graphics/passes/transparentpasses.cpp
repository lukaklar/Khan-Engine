#include "graphics/precomp.h"
#include "graphics/passes/transparentpasses.hpp"
#include "graphicshal/physicalrenderpass.hpp"
#include "graphicshal/pixelformats.hpp"
#include "graphicshal/queuetype.hpp"
#include "graphicshal/renderbackend.hpp"
#include "graphicshal/rendercontext.hpp"
#include "graphicshal/renderdevice.hpp"
#include "graphicshal/texture.hpp"
#include "graphicshal/textureview.hpp"
#include "graphics/renderer.hpp"
#include "graphics/rendergraph.hpp"

namespace Khan
{
	TransparentPass::TransparentPass()
		: RenderPass(QueueType_Graphics, "TransparentPass")
	{
		PhysicalRenderPassDescription desc;
		desc.m_RenderTargetCount = 1;
		desc.m_RenderTargets[0].m_Format = PF_R16G16B16A16_FLOAT;
		desc.m_RenderTargets[0].m_StartAccess = StartAccessType::Keep;
		desc.m_RenderTargets[0].m_EndAccess = EndAccessType::Keep;
		desc.m_DepthStencil.m_Format = PF_D32_FLOAT;
		desc.m_DepthStencil.m_DepthStartAccess = StartAccessType::Keep;
		desc.m_DepthStencil.m_DepthEndAccess = EndAccessType::Keep;

		m_PhysicalRenderPass = RenderBackend::g_Device->CreatePhysicalRenderPass(desc);
	}

	void TransparentPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		TextureViewDesc viewDesc;
		viewDesc.m_Type = TextureViewType_2D;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;

		viewDesc.m_Format = renderer.GetResourceBoard().m_Transient.m_LightAccumulationBuffer->GetDesc().m_Format;
		m_ColorBuffer = renderGraph.DeclareResourceDependency(renderer.GetResourceBoard().m_Transient.m_LightAccumulationBuffer, viewDesc, ResourceState_RenderTarget, true);

		viewDesc.m_Format = PF_D32_FLOAT;
		m_DepthBuffer = renderGraph.DeclareResourceDependency(renderer.GetResourceBoard().m_Transient.m_GBuffer.m_Depth, viewDesc, ResourceState_DepthWriteStencilWrite);
	}

	void TransparentPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.BeginPhysicalRenderPass(*m_PhysicalRenderPass, &m_ColorBuffer, m_DepthBuffer);

		/*auto& meshes = renderer.GetTransparentMeshes();
		for (auto* mesh : meshes)
		{
			context.SetVertexBuffer(0, mesh->GetVertexBuffer(), 0);
			context.SetIndexBuffer(mesh->GetIndexBuffer(), 0, false);

			auto& submeshes = mesh->GetSubMeshData();
			for (auto& submesh : submeshes)
			{
				Material* material = submesh.m_Material;

				if (!material->IsCompiled())
				{
					m_PipelineDesc.m_PixelShader = material->GetPixelShader();
					m_PipelineDesc.m_RasterizerState.m_CullMode = material->HasTwoSides() ? RasterizerState::CullMode::None : RasterizerState::CullMode::Back;

					material->SetPipelineState(context.GetDevice().CreateGraphicsPipelineState(m_PipelineDesc));
				}

				context.SetPipelineState(*material->GetPipelineState());

				auto& textures = material->GetTextures();
				for (auto& texture : textures)
				{
					context.SetSRVTexture(ResourceBindFrequency_PerMaterial, texture.m_Binding, texture.m_Texture);
				}

				context.DrawIndexedInstanced(submesh.m_NumIndices, 1, submesh.m_IndexBufferOffset, submesh.m_VertexBufferOffset, 0);
			}
		}*/

		context.EndPhysicalRenderPass();
	}
}