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
			desc.m_DepthStencilState.m_DepthMode.m_DepthBoundsTestEnabled = true;
			desc.m_DepthStencilState.m_DepthMode.m_DepthWriteEnabled = true;
			desc.m_DepthStencilState.m_DepthMode.m_DepthFunc = DepthStencilState::CompareFunction::Less;
			desc.m_RasterizerState.m_CullMode = RasterizerState::CullMode::Back;
			desc.m_PhysicalRenderPass = m_PhysicalRenderPass;

			m_PipelineState = RenderBackend::g_Device->CreateGraphicsPipelineState(desc);
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
		renderer.GetResourceBlackboard().GBuffer.Depth = depthBuffer;

		TextureViewDesc viewDesc;
		viewDesc.m_Type = TextureViewType_2D;
		viewDesc.m_Format = renderer.GetResourceBlackboard().m_FinalOutput->GetDesc().m_Format;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;

		m_DepthBuffer = renderGraph.DeclareResourceDependency(depthBuffer, viewDesc, ResourceState_DepthWriteStencilWrite);
	}

	void DepthPrePass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.BeginPhysicalRenderPass(*m_PhysicalRenderPass, nullptr, m_DepthBuffer);
		context.SetPipelineState(*m_PipelineState);
		context.SetViewport(0.0f, 0.0f, (float)m_DepthBuffer->GetTexture().GetDesc().m_Width, (float)m_DepthBuffer->GetTexture().GetDesc().m_Height);
		context.SetScissor(0, 0, m_DepthBuffer->GetTexture().GetDesc().m_Width, m_DepthBuffer->GetTexture().GetDesc().m_Height);
		
		auto& meshes = renderer.GetOpaqueMeshes();
		for (auto* mesh : meshes)
		{
			context.SetVertexBuffer(0, mesh->GetVertexBuffer(), 0);
			context.SetIndexBuffer(mesh->GetIndexBuffer(), 0, false);

			auto& submeshes = mesh->GetSubMeshData();
			for (auto& submesh : submeshes)
			{
				context.DrawIndexedInstanced(submesh.m_NumIndices, 1, submesh.m_IndexBufferOffset, submesh.m_VertexBufferOffset, 0);
			}
		}

		context.EndPhysicalRenderPass();
	}
}