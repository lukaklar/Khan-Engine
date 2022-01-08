#pragma once
#include "core/camera/camera.hpp"
#include "graphics/hal/constantbuffer.hpp"
#include "graphics/passes/clusterpasses.hpp"
#include "graphics/passes/deferredpasses.hpp"
#include "graphics/passes/depthpasses.hpp"
#include "graphics/passes/finalpasses.hpp"
#include "graphics/passes/gbufferpasses.hpp"
#include "graphics/passes/transparentpasses.hpp"
#include "graphics/posteffects/fxaa.hpp"
#include "graphics/posteffects/hdr.hpp"
#include "graphics/posteffects/ssao.hpp"
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
		float m_Padding[3];
	};

	struct FrustumParams
	{
		glm::mat4 m_Projection;
		glm::mat4 m_InverseProjection;
		glm::vec2 m_ScreenDimensions;
		float m_Near;
		float m_Far;
		glm::vec3 m_ClusterCount;
		float m_TileSize;
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

		inline ConstantBuffer& GetFrustumParams() { return m_FrustumParams; }

		inline uint32_t GetTotalNumClusters() const { return m_TotalNumClusters; }

		inline uint32_t GetScreenTileSize() const { return K_TILE_SIZE; }
		inline uint32_t GetNumDepthSlices() const { return K_NUM_DEPTH_SLICES; }

	private:
		void SchedulePasses();

		void RecreateClustersBuffer();
		void DestroyClustersBuffer();

		DepthPrePass m_DepthPrePass;
		GBufferPass m_GBufferPass;
		LightDataUploadPass m_LightDataUploadPass;
		ClusterCalculationPass m_ClusterCalculationPass;
		MarkActiveClustersPass m_MarkActiveClustersPass;
		CompactActiveClustersPass m_CompactActiveClustersPass;
		LightCullingPass m_LightCullingPass;
		ClusterDeferredLightingPass m_DeferredLightingPass;
		//DeferredLightingPass m_DeferredLightingPass;
		//TransparentPass m_TransparentPass;
		SSAOPass m_SSAOPass;
		HDRPass m_HDRPass;
		FXAAPass m_FXAAPass;
		FinalPass m_FinalPass;

		ResourceBoard m_ResourceBoard;
		ThreadPool m_ThreadPool;

		std::vector<Mesh*> m_OpaqueMeshes;
		//std::map<float, Mesh*> m_TransparentMeshes;

		std::vector<ShaderLightData> m_ActiveLightData;

		const Camera* m_ActiveCamera;

		ConstantBuffer m_FrustumParams;

		uint32_t m_TotalNumClusters;
		bool m_FrustumParamsChanged;

		inline static constexpr uint32_t K_TILE_SIZE = 16;
		inline static constexpr uint32_t K_NUM_DEPTH_SLICES = 32;
	};
}