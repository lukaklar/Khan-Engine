#pragma once

#ifdef KH_DEBUG
#include <string>
#endif // KH_DEBUG

namespace Khan
{
	enum QueueType;
	class RenderContext;
	class Renderer;
	class RenderGraph;

	class RenderPass
	{
	public:
		RenderPass(QueueType executionQueue, const char* name)
			: m_ExecutionQueue(executionQueue)
#ifdef KH_DEBUG
			, m_Name(name)
#endif // KH_DEBUG
		{
		}

		virtual ~RenderPass()
		{
		}

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) = 0;
		virtual void Execute(RenderContext& context, Renderer& renderer) = 0;

		inline QueueType GetExecutionQueue() const { return m_ExecutionQueue; }

#ifdef KH_DEBUG
		inline const std::string& GetName() const { return m_Name; }
#endif // KH_DEBUG

	private:
		QueueType m_ExecutionQueue;
		// TODO: Add enabled flag

#ifdef KH_DEBUG
		std::string m_Name;
#endif // KH_DEBUG
	};
}