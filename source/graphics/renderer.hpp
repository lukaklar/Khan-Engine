#pragma once
#include "core/camera/camera.hpp"
#include "graphics/hal/constantbuffer.hpp"
#include "graphics/passes/depthpasses.hpp"
#include "graphics/passes/finalpasses.hpp"
#include "graphics/passes/gbufferpasses.hpp"
#include "graphics/passes/testpasses.hpp"
#include "graphics/passes/tileddeferredpasses.hpp"
#include "graphics/passes/transparentpasses.hpp"
#include "graphics/posteffects/hdr.hpp"
#include "graphics/resourceboard.hpp"
#include "system/threadpool.hpp"

namespace Khan
{
	class Buffer;
	class BufferView;
	struct Mesh;

	struct ShaderLightData
	{
		uint32_t m_Type;
		glm::vec3 m_PositionVS;
		glm::vec3 m_DirectionVS;
		float m_Range;
		glm::vec3 m_Color;
		float m_Luminance;
		float m_SpotlightAngle;
	};

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void PreRender();
		void Render();
		void PostRender();

		inline ResourceBoard& GetResourceBoard() { return m_ResourceBoard; }
		inline ThreadPool& GetThreadPool() { return m_ThreadPool; }

		inline std::vector<Mesh*>& GetOpaqueMeshes() { return m_OpaqueMeshes; }
		//inline std::map<float, Mesh*>& GetTransparentMeshes() { return m_TransparentMeshes; }

		inline std::vector<ShaderLightData>& GetActiveLightData() { return m_ActiveLightData; }

		inline const Camera* GetActiveCamera() const { return m_ActiveCamera; }
		inline void SetActiveCamera(const Camera* camera) { m_ActiveCamera = camera; }

		inline ConstantBuffer& GetTiledDeferredDispatchParams() { return m_TiledDeferredDispatchParams; }
		inline ConstantBuffer& GetScreenToViewParams() { return m_ScreenToViewParams; }

		inline uint32_t GetScreenTileSize() const { return K_TILE_SIZE; }

		inline const glm::uvec3& GetNumDispatchThreadGroups() const { return m_NumDispatchThreadGroups; }

	private:
		void SchedulePasses();

		void RecreateScreenFrustumBuffer();
		void DestroyScreenFrustumBuffer();

		DepthPrePass m_DepthPrePass;
		GBufferPass m_GBufferPass;
		LightDataUploadPass m_LightDataUploadPass;
		TileFrustumCalculationPass m_TileFrustumCalculationPass;
		LightCullingPass m_LightCullingPass;
		TiledDeferredLightingPass m_TiledDeferredLightingPass;
		//TransparentPass m_TransparentPass;
		HDRPass m_HDRPass;
		//TestPass m_TestPass;
		FinalPass m_FinalPass;

		ResourceBoard m_ResourceBoard;
		ThreadPool m_ThreadPool;

		std::vector<Mesh*> m_OpaqueMeshes;
		//std::map<float, Mesh*> m_TransparentMeshes;

		std::vector<ShaderLightData> m_ActiveLightData;

		const Camera* m_ActiveCamera;

		bool m_ScreenDimensionsChanged;

		glm::uvec3 m_NumDispatchThreadGroups;
		glm::uvec3 m_NumDispatchThreads;

		ConstantBuffer m_TiledDeferredDispatchParams;
		ConstantBuffer m_ScreenToViewParams;

		inline static constexpr uint32_t K_TILE_SIZE = 16;
	};
}