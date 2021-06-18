#pragma once

namespace Khan
{
	class RenderContext;
	class Renderer;
	class RenderPass;

	class RenderTask
	{
	public:
		RenderTask(RenderPass& renderPass, RenderContext& context, Renderer& renderer);

		void operator()();

	private:
		RenderPass& m_RenderPass;
		RenderContext& m_Context;
		Renderer& m_Renderer;
	};
}