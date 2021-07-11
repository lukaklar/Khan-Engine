#pragma once
#include "graphics/hal/constantbuffer.hpp"
#include "graphics/passes/depthpasses.hpp"
#include "graphics/passes/finalpasses.hpp"
#include "graphics/passes/gbufferpasses.hpp"
#include "graphics/passes/tileddeferredpasses.hpp"
#include "graphics/passes/transparentpasses.hpp"
#include "graphics/posteffects/hdr.hpp"
#include "graphics/passes/testpasses.hpp"
#include "graphics/resourceblackboard.hpp"
#include "system/threadpool.hpp"
#include <thirdparty/glm/glm.hpp>

namespace Khan
{
	class Buffer;
	class BufferView;
	class Mesh;

	class Renderer
	{
	public:
		Renderer();

		void PreRender();
		void Render();
		void PostRender();

		inline ResourceBlackboard& GetResourceBlackboard() { return m_ResourceBlackboard; }
		inline ThreadPool& GetThreadPool() { return m_ThreadPool; }

		inline std::vector<Mesh*>& GetOpaqueMeshes() { return m_OpaqueMeshes; }
		//inline std::map<float, Mesh*>& GetTransparentMeshes() { return m_TransparentMeshes; }

		inline uint32_t GetScreenTileSize() const { return K_TILE_SIZE; }

	private:
		void SchedulePasses();
		void RecreateScreenFrustumBuffer();

		//TileFrustumCalculationPass m_TileFrustumCalculationPass;
		//DepthPrePass m_DepthPrePass;
		//GBufferPass m_GBufferPass;
		//LightCullingPass m_LightCullingPass;
		//TiledDeferredLightingPass m_TiledDeferredLightingPass;
		//TransparentPass m_TransparentPass;
		//HDRPass m_HDRPass;
		TestPass m_TestPass;
		FinalPass m_FinalPass;

		ResourceBlackboard m_ResourceBlackboard;
		ThreadPool m_ThreadPool;

		std::vector<Mesh*> m_OpaqueMeshes;
		//std::map<float, Mesh*> m_TransparentMeshes;

		uint32_t m_ScreenWidth, m_ScreenHeight;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_InverseViewMatrix;
		glm::mat4 m_InverseProjectionMatrix;

		bool m_ScreenSizeChanged;

		inline static constexpr uint32_t K_TILE_SIZE = 16;
	};
}