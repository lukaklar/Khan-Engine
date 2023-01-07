#include "graphics/precomp.h"
#include "graphics/rendertask.hpp"
#include "graphics/renderpass.hpp"
#include "graphicshal/rendercontext.hpp"

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