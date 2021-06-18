#pragma once
#include "graphics/passes/deferredpasses.hpp"
#include "graphics/passes/gbufferpasses.hpp"
#include "graphics/passes/transparentpasses.hpp"
#include "graphics/posteffects/hdr.hpp"
#include "graphics/passes/testpasses.hpp"
#include "graphics/resourceblackboard.hpp"
#include "system/threadpool.hpp"

namespace Khan
{
	class Renderer
	{
	public:
		void PreRender();
		void Render();
		void PostRender();

		inline ResourceBlackboard& GetResourceBlackboard() { return m_ResourceBlackboard; }
		inline ThreadPool& GetThreadPool() { return m_ThreadPool; }

	private:
		void SchedulePasses();

		GBufferPass m_GBufferPass;
		DeferredLightingPass m_DeferredLightingPass;
		TransparentPass m_TransparentPass;
		HDRPass m_HDRPass;
		TestPass m_TestPass;

		ResourceBlackboard m_ResourceBlackboard;
		ThreadPool m_ThreadPool;
	};
}