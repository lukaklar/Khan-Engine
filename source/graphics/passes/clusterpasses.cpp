#include "graphics/precomp.h"
#include "graphics/passes/clusterpasses.hpp"
#include "graphics/objects/mesh.hpp"
#include "graphics/renderer.hpp"
#include "graphics/rendergraph.hpp"
#include "graphics/shadermanager.hpp"
#include "graphicshal/buffer.hpp"
#include "graphicshal/bufferview.hpp"
#include "graphicshal/physicalrenderpass.hpp"
#include "graphicshal/pixelformats.hpp"
#include "graphicshal/queuetype.hpp"
#include "graphicshal/renderbackend.hpp"
#include "graphicshal/rendercontext.hpp"
#include "graphicshal/resourcebindfrequency.hpp"
#include "graphicshal/texture.hpp"
#include "graphicshal/textureview.hpp"
#include "core/defines.h"

namespace Khan
{
	LightDataUploadPass::LightDataUploadPass()
		: RenderPass(QueueType_Copy, "LightDataUploadPass")
	{
	}

	void LightDataUploadPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		renderGraph.EnableDMA(true);

		BufferDesc desc;
		desc.m_Size = static_cast<uint32_t>(renderer.GetActiveLightData().size() * sizeof(ShaderLightData));
		desc.m_Flags = BufferFlag_AllowUnorderedAccess | BufferFlag_AllowShaderResource | BufferFlag_Writable;

		Buffer* temp = renderGraph.CreateManagedResource(desc);
		KH_DEBUGONLY(temp->SetDebugName("Active Scene Lights"));
		renderer.GetResourceBoard().m_Transient.m_ActiveSceneLights = temp;

		BufferViewDesc viewDesc;
		viewDesc.m_Offset = 0;
		viewDesc.m_Range = desc.m_Size;
		viewDesc.m_Format = PF_NONE;

		m_LightData = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_CopyDestination);
	}

	void LightDataUploadPass::Execute(RenderContext& context, Renderer& renderer)
	{
		std::vector<ShaderLightData>& lights = renderer.GetActiveLightData();
		uint32_t sizeInBytes = static_cast<uint32_t>(lights.size() * sizeof(ShaderLightData));

		context.UpdateBufferFromHost(m_LightData, lights.data());
	}

	ClusterCalculationPass::ClusterCalculationPass()
		: RenderPass(QueueType_Compute, "ClusterCalculationPass")
	{
		ComputePipelineDescription desc;
		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("clustercalculation_CS", "CS_ComputeCluster");

		m_PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void ClusterCalculationPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		renderGraph.EnableAsyncCompute(true);

		Buffer* temp = renderer.GetResourceBoard().m_Persistent.m_Clusters;

		BufferViewDesc desc;
		desc.m_Offset = 0;
		desc.m_Range = temp->GetDesc().m_Size;
		desc.m_Format = PF_NONE;

		m_Clusters = renderGraph.DeclareResourceDependency(temp, desc, ResourceState_UnorderedAccess);
	}

	void ClusterCalculationPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.SetPipelineState(*m_PipelineState);
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &renderer.GetFrustumParams());
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 0, m_Clusters);

		glm::vec2 threadGroupsXY = { renderer.GetActiveCamera()->GetViewportWidth(), renderer.GetActiveCamera()->GetViewportHeight() };
		threadGroupsXY = glm::ceil(threadGroupsXY / (float)renderer.GetScreenTileSize());
		threadGroupsXY = glm::ceil(threadGroupsXY / (float)renderer.GetScreenTileSize());
		context.Dispatch((uint32_t)threadGroupsXY.x, (uint32_t)threadGroupsXY.y, renderer.GetNumDepthSlices());
	}

	MarkActiveClustersPass::MarkActiveClustersPass()
		: RenderPass(QueueType_Graphics, "MarkActiveClustersPass")
		, m_PerFrameConsts(sizeof(glm::mat4))
	{
		{
			PhysicalRenderPassDescription desc;
			desc.m_DepthStencil.m_Format = PF_D32_FLOAT;
			desc.m_DepthStencil.m_DepthStartAccess = StartAccessType::Keep;
			desc.m_DepthStencil.m_DepthEndAccess = EndAccessType::Keep;

			m_PhysicalRenderPass = RenderBackend::g_Device->CreatePhysicalRenderPass(desc);
		}

		{
			GraphicsPipelineDescription desc;
			desc.m_VertexShader = ShaderManager::Get()->GetShader<ShaderType_Vertex>("markactiveclusters_VS", "VS_MarkActiveClusters");
			desc.m_PixelShader = ShaderManager::Get()->GetShader<ShaderType_Pixel>("markactiveclusters_PS", "PS_MarkActiveClusters");
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::POSITION);
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float2, VertexInputState::StreamDescriptor::StreamElement::Usage::TEXCOORD0);
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::NORMAL);
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::TANGENT);
			desc.m_VertexInputState.AddStreamElement(0, VertexInputState::StreamDescriptor::StreamElement::Type::Float3, VertexInputState::StreamDescriptor::StreamElement::Usage::BITANGENT);
			desc.m_DepthStencilState.m_DepthMode.m_DepthTestEnabled = true;
			desc.m_DepthStencilState.m_DepthMode.m_DepthFunc = DepthStencilState::CompareFunction::LessOrEqual;
			desc.m_PhysicalRenderPass = m_PhysicalRenderPass;

			m_PipelineState = RenderBackend::g_Device->CreateGraphicsPipelineState(desc);
		}
	}

	void MarkActiveClustersPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		{
			Buffer* temp;
			BufferDesc desc;
			BufferViewDesc viewDesc;

			desc.m_Size = renderer.GetTotalNumClusters() * sizeof(uint32_t);
			desc.m_Flags = BufferFlag_AllowShaderResource | BufferFlag_AllowUnorderedAccess | BufferFlag_Writable;

			temp = renderGraph.CreateManagedResource(desc);
			KH_DEBUGONLY(temp->SetDebugName("Active Cluster Flags"));
			renderer.GetResourceBoard().m_Transient.m_ActiveClusterFlags = temp;

			viewDesc.m_Offset = 0;
			viewDesc.m_Range = temp->GetDesc().m_Size;
			viewDesc.m_Format = PF_R32_UINT;
			m_ActiveClusterFlags = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_CopyDestination);
		}

		{
			Texture* temp;
			TextureViewDesc viewDesc;

			temp = renderer.GetResourceBoard().m_Transient.m_GBuffer.m_Depth;
			viewDesc.m_Type = TextureViewType_2D;
			viewDesc.m_Format = temp->GetDesc().m_Format;
			viewDesc.m_BaseArrayLayer = 0;
			viewDesc.m_LayerCount = 1;
			viewDesc.m_BaseMipLevel = 0;
			viewDesc.m_LevelCount = 1;
			m_DepthTexture = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_DepthWriteStencilWrite, true);
		}
	}

	void MarkActiveClustersPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.ClearBuffer(m_ActiveClusterFlags, 0);
		context.BeginPhysicalRenderPass(*m_PhysicalRenderPass, nullptr, m_DepthTexture);
		context.SetPipelineState(*m_PipelineState);
		context.SetViewport(0.0f, 0.0f, (float)renderer.GetActiveCamera()->GetViewportWidth(), (float)renderer.GetActiveCamera()->GetViewportHeight());
		context.SetScissor(0, 0, renderer.GetActiveCamera()->GetViewportWidth(), renderer.GetActiveCamera()->GetViewportHeight());
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &renderer.GetFrustumParams());
		m_PerFrameConsts.UpdateConstantData(&renderer.GetActiveCamera()->GetViewProjection(), 0, sizeof(glm::mat4));
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 1, &m_PerFrameConsts);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 0, m_ActiveClusterFlags);

		for (auto* mesh : renderer.GetOpaqueMeshes())
		{
			context.SetVertexBuffer(0, mesh->m_VertexBuffer, 0);
			context.SetIndexBuffer(mesh->m_IndexBuffer, 0, false);

			context.SetConstantBuffer(ResourceBindFrequency_PerDraw, 0, &mesh->m_ParentTransform);

			context.DrawIndexedInstanced(mesh->m_IndexCount, 1, 0, 0, 0);
		}

		context.EndPhysicalRenderPass();
	}

	CompactActiveClustersPass::CompactActiveClustersPass()
		: RenderPass(QueueType_Compute, "CompactActiveClustersPass")
		, m_ClusterParams(sizeof(uint32_t))
	{
		ComputePipelineDescription desc;
		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("compactactiveclusters_CS", "CS_CompactActiveClusters");

		m_PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void CompactActiveClustersPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		renderGraph.EnableAsyncCompute(true);

		Buffer* temp;
		BufferDesc desc;
		BufferViewDesc viewDesc;

		temp = renderer.GetResourceBoard().m_Transient.m_ActiveClusterFlags;
		viewDesc.m_Offset = 0;
		viewDesc.m_Range = temp->GetDesc().m_Size;
		viewDesc.m_Format = PF_R32_UINT;
		m_ActiveClusterFlags = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

		desc.m_Size = sizeof(uint32_t) * renderer.GetTotalNumClusters();
		desc.m_Flags = BufferFlag_AllowUnorderedAccess | BufferFlag_AllowShaderResource;
		temp = renderGraph.CreateManagedResource(desc);
		KH_DEBUGONLY(temp->SetDebugName("Active Cluster Index List"));
		renderer.GetResourceBoard().m_Transient.m_ActiveClusterIndexList = temp;

		viewDesc.m_Range = desc.m_Size;
		m_ActiveClusterIndexList = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

		desc.m_Size = 3 * sizeof(uint32_t);
		desc.m_Flags = BufferFlag_AllowUnorderedAccess | BufferFlag_AllowIndirect;
		temp = renderGraph.CreateManagedResource(desc);
		KH_DEBUGONLY(temp->SetDebugName("Culling Dispatch Args"));
		renderer.GetResourceBoard().m_Transient.m_CullingDispatchArgs = temp;

		viewDesc.m_Range = desc.m_Size;
		viewDesc.m_Format = PF_R32_UINT;
		m_CullingDispatchArgs = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);
	}

	void CompactActiveClustersPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.SetPipelineState(*m_PipelineState);

		uint32_t numClusters = static_cast<uint32_t>(renderer.GetTotalNumClusters());
		m_ClusterParams.UpdateConstantData(&numClusters, 0, sizeof(uint32_t));
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &m_ClusterParams);

		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 0, m_ActiveClusterFlags);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 0, m_ActiveClusterIndexList);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 1, m_CullingDispatchArgs);

		uint32_t threadGroupCountX = (uint32_t)glm::ceil(numClusters / 64.0f);
		context.Dispatch(threadGroupCountX, 1, 1);
	}

	LightCullingPass::LightCullingPass()
		: RenderPass(QueueType_Compute, "LightCullingPass")
		, m_LightParams(sizeof(uint32_t))
	{
		ComputePipelineDescription desc;
		desc.m_ComputeShader = ShaderManager::Get()->GetShader<ShaderType_Compute>("clusterlightculling_CS", "CS_CullLights");

		m_PipelineState = RenderBackend::g_Device->CreateComputePipelineState(desc);
	}

	void LightCullingPass::Setup(RenderGraph& renderGraph, Renderer& renderer)
	{
		renderGraph.EnableAsyncCompute(true);

		Buffer* temp;
		BufferDesc desc;
		BufferViewDesc viewDesc;

		temp = renderer.GetResourceBoard().m_Persistent.m_Clusters;
		viewDesc.m_Offset = 0;
		viewDesc.m_Range = temp->GetDesc().m_Size;
		viewDesc.m_Format = PF_NONE;
		m_Clusters = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

		temp = renderer.GetResourceBoard().m_Transient.m_ActiveClusterIndexList;
		viewDesc.m_Range = temp->GetDesc().m_Size;
		viewDesc.m_Format = PF_R32_UINT;
		m_ActiveClusterIndexList = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

		temp = renderer.GetResourceBoard().m_Transient.m_ActiveSceneLights;
		viewDesc.m_Range = temp->GetDesc().m_Size;
		viewDesc.m_Format = PF_NONE;
		m_Lights = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_NonPixelShaderAccess);

		desc.m_Size = sizeof(uint32_t);
		desc.m_Flags = BufferFlag_AllowUnorderedAccess;
		temp = renderGraph.CreateManagedResource(desc);
		KH_DEBUGONLY(temp->SetDebugName("Light Index Counter"));

		viewDesc.m_Range = desc.m_Size;
		m_LightIndexCounter = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

		desc.m_Size = 720000 * sizeof(uint32_t);
		desc.m_Flags = BufferFlag_AllowUnorderedAccess | BufferFlag_AllowShaderResource;
		temp = renderGraph.CreateManagedResource(desc);
		KH_DEBUGONLY(temp->SetDebugName("Light Index List"));
		renderer.GetResourceBoard().m_Transient.m_LightIndexList = temp;

		viewDesc.m_Range = desc.m_Size;
		viewDesc.m_Format = PF_R32_UINT;
		m_LightIndexList = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

		desc.m_Size = 2 * sizeof(uint32_t) * renderer.GetTotalNumClusters();
		desc.m_Flags = BufferFlag_AllowUnorderedAccess | BufferFlag_AllowShaderResource;
		temp = renderGraph.CreateManagedResource(desc);
		KH_DEBUGONLY(temp->SetDebugName("Light Grid"));
		renderer.GetResourceBoard().m_Transient.m_LightGrid = temp;

		viewDesc.m_Range = desc.m_Size;
		viewDesc.m_Format = PF_R32G32_UINT;
		m_LightGrid = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_UnorderedAccess);

		temp = renderer.GetResourceBoard().m_Transient.m_CullingDispatchArgs;
		viewDesc.m_Range = temp->GetDesc().m_Size;
		m_IndirectDispatchArgs = renderGraph.DeclareResourceDependency(temp, viewDesc, ResourceState_IndirectArgument);
	}

	void LightCullingPass::Execute(RenderContext& context, Renderer& renderer)
	{
		context.SetPipelineState(*m_PipelineState);

		uint32_t numLights = static_cast<uint32_t>(renderer.GetActiveLightData().size());
		m_LightParams.UpdateConstantData(&numLights, 0, sizeof(uint32_t));
		context.SetConstantBuffer(ResourceBindFrequency_PerFrame, 0, &m_LightParams);

		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 0, m_Clusters);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 1, m_ActiveClusterIndexList);
		context.SetSRVBuffer(ResourceBindFrequency_PerFrame, 2, m_Lights);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 0, m_LightIndexCounter);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 1, m_LightIndexList);
		context.SetUAVBuffer(ResourceBindFrequency_PerFrame, 2, m_LightGrid);

		context.DispatchIndirect(m_IndirectDispatchArgs, 0);
	}
}