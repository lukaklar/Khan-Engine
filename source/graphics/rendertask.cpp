#include "graphics/precomp.h"
#include "graphics/rendertask.hpp"
#include "graphics/renderpass.hpp"

#ifdef KH_GFXAPI_VULKAN
#include "graphics/hal/vulkan/vulkancontext.hpp"
#endif // KH_GFXAPI_VULKAN

namespace Khan
{
	RenderTask::RenderTask(RenderPass& renderPass, RenderContext& context, Renderer& renderer)
		: m_RenderPass(renderPass)
		, m_Context(context)
		, m_Renderer(renderer)
	{
	}

	void RenderTask::operator()()
	{
		m_Context.BeginRecording(m_RenderPass);
		m_RenderPass.Execute(m_Context, m_Renderer);
		m_Context.EndRecording();
	}
}