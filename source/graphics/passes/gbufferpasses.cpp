#include "graphics/precomp.h"
#include "graphics/passes/gbufferpasses.hpp"
#include "graphics/hal/physicalrenderpass.hpp"
#include "graphics/hal/pixelformats.hpp"
#include "graphics/hal/queuetype.hpp"
#include "graphics/hal/renderbackend.hpp"
#include "graphics/hal/rendercontext.hpp"
#include "graphics/hal/renderdevice.hpp"
#include "graphics/hal/resourcebindfrequency.hpp"
#include "graphics/hal/texture.hpp"
#include "graphics/hal/textureview.hpp"
#include "graphics/renderer.hpp"
#include "graphics/rendergraph.hpp"
#include "core/defines.h"
#include "graphics/objects/mesh.hpp"
#include "graphics/materials/material.hpp"

#define DECLARE_GBUFFER_OUTPUT(target, format, state)\
desc.m_Format = format;\
temp = renderGraph.CreateManagedResource(desc);\
KH_DEBUGONLY(temp->SetDebugName(#target);)\
renderer.GetResourceBoard().m_Transient.m_GBuffer.m_##target = temp;\
viewDesc.m_Format = format;\
m_GBuffer_##target = renderGraph.DeclareResourceDependency(temp, viewDesc, state);

#define POPULATE_RENDER_TARGET_DATA(index, pixelFormat, startAccess, endAccess)\
desc.m_RenderTargets[index].m_Format = pixelFormat;\
desc.m_RenderTargets[index].m_StartAccess = startAccess;\
desc.m_RenderTargets[index].m_EndAccess = endAccess;\

namespace Khan
{
	GBufferPass::GBufferPass()
		: RenderPass(QueueType_Graphics, "GBufferPass")
	{
		PhysicalRenderPassDescription desc;
		desc.m_RenderTargetCount = 6;
		POPULATE_RENDER_TARGET_DATA(0, PF_R8G8B8A8_SRGB, StartAccessType::Clear, EndAccessType::Keep);
		POPULATE_RENDER_TARGET_DATA(1, PF_R11G11B10_FLOAT, StartAccessType::Clear, EndAccessType::Keep);
		POPULATE_RENDER_TARGET_DATA(2, PF_R11G11B10_FLOAT, StartAccessType::Clear, EndAccessType::Keep);
		POPULATE_RENDER_TARGET_DATA(3, PF_R8G8B8A8_SRGB, StartAccessType::Clear, EndAccessType::Keep);
		POPULATE_RENDER_TARGET_DATA(4, PF_R16G16_FLOAT, StartAccessType::Clear, EndAccessType::Keep);
		POPULATE_RENDER_TARGET_DATA(5, PF_R16G16_FLOAT, StartAccessType::Clear, EndAccessType::Keep);
		desc.m_DepthStencil.m_Format = PF_D32_FLOAT;
		desc.m_DepthStencil.m_DepthStartAccess = StartAccessType::Keep;
		desc.m_DepthStencil.m_DepthEndAccess = EndAccessType::Keep;

		m_PipelineDesc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::POSITION);
		m_PipelineDesc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float2, VertexInputState::StreamDescriptor::StreamElement::Usage::TEXCOORD0);
		m_PipelineDesc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::NORMAL);
		m_PipelineDesc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::BITANGENT);
		m_PipelineDesc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::TANGENT);
		m_PipelineDesc.m_DepthStencilState.m_DepthMode.m_DepthTestEnabled = true;
		m_PipelineDesc.m_DepthStencilState.m_DepthMode.m_DepthWriteEnabled = true;
		m_PipelineDesc.m_DepthStencilState.m_DepthMode.m_DepthFunc = DepthStencilState::CompareFunction::LessOrEqual;
		m_PipelineDesc.m_PhysicalRenderPass = RenderBackend::g_Device->CreatePhysicalRenderPass(desc);
	}

	void GBufferPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		Texture* temp;

		TextureDesc desc;
		desc.m_Type = TextureType_2D;
		desc.m_Width = 1280;
		desc.m_Height = 720;
		desc.m_Depth = 1;
		desc.m_ArrayLayers = 1;
		desc.m_MipLevels = 1;
		//desc.m_SampleCount;
		desc.m_Flags = TextureFlag_AllowShaderResource | TextureFlag_AllowRenderTarget;

		TextureViewDesc viewDesc;
		viewDesc.m_Type = TextureViewType_2D;
		viewDesc.m_BaseArrayLayer = 0;
		viewDesc.m_LayerCount = 1;
		viewDesc.m_BaseMipLevel = 0;
		viewDesc.m_LevelCount = 1;

		DECLARE_GBUFFER_OUTPUT(Albedo, PF_R8G8B8A8_SRGB, ResourceState_RenderTarget);
		DECLARE_GBUFFER_OUTPUT(Normals, PF_R11G11B10_FLOAT, ResourceState_RenderTarget);
		DECLARE_GBUFFER_OUTPUT(Emissive, PF_R11G11B10_FLOAT, ResourceState_RenderTarget);
		DECLARE_GBUFFER_OUTPUT(SpecularReflectance, PF_R8G8B8A8_SRGB, ResourceState_RenderTarget);
		DECLARE_GBUFFER_OUTPUT(MetallicAndRoughness, PF_R16G16_FLOAT, ResourceState_RenderTarget);
		//DECLARE_GBUFFER_OUTPUT(MotionVectors, PF_R16G16_FLOAT, ResourceState_RenderTarget);
		
		viewDesc.m_Format = PF_D32_FLOAT;
		m_GBuffer_Depth = renderGraph.DeclareResourceDependency(renderer.GetResourceBoard().m_Transient.m_GBuffer.m_Depth, viewDesc, ResourceState_DepthWriteStencilWrite);
	}

	void GBufferPass::Execute(RenderContext& context, Renderer& renderer)
	{
		TextureView* renderTargets[] =
		{
			m_GBuffer_Albedo,
			m_GBuffer_Normals,
			m_GBuffer_Emissive,
			m_GBuffer_SpecularReflectance,
			m_GBuffer_MetallicAndRoughness,
			//m_GBuffer_MotionVectors
		};

		context.BeginPhysicalRenderPass(*m_PipelineDesc.m_PhysicalRenderPass, renderTargets, m_GBuffer_Depth);

		auto& meshes = renderer.GetOpaqueMeshes();
		for (auto* mesh : meshes)
		{
			Material* material = mesh->m_Material;

			if (!material->IsCompiled())
			{
				m_PipelineDesc.m_PixelShader = material->GetPixelShader();
				m_PipelineDesc.m_RasterizerState.m_CullMode = material->HasTwoSides() ? RasterizerState::CullMode::None : RasterizerState::CullMode::Back;

				material->SetPipelineState(context.GetDevice().CreateGraphicsPipelineState(m_PipelineDesc));
			}

			context.SetPipelineState(*material->GetPipelineState());

			for (auto& texture : material->GetTextures())
			{
				context.SetSRVTexture(ResourceBindFrequency_PerMaterial, texture.m_Binding, texture.m_Texture);
			}

			context.SetVertexBuffer(0, mesh->m_VertexBuffer, 0);
			context.SetIndexBuffer(mesh->m_IndexBuffer, 0, false);

			context.DrawIndexedInstanced(mesh->m_IndexCount, 1, 0, 0, 0);
		}

		context.EndPhysicalRenderPass();
	}
}