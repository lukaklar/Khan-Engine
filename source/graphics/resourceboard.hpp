#pragma once
#include <algorithm>

namespace Khan
{
	class Buffer;
	class Texture;
	class RenderGraph;

	struct ResourceBoard
	{
		struct
		{
			Buffer* m_Clusters = nullptr;
			Buffer* m_AdaptedLuminance = nullptr;
			Texture* m_FinalOutput = nullptr;
		} m_Persistent;

		struct
		{
			Buffer* m_ActiveClusterFlags = nullptr;
			Buffer* m_ActiveClusterIndexList = nullptr;
			Buffer* m_CullingDispatchArgs = nullptr;

			Buffer* m_ActiveSceneLights = nullptr;
			Buffer* m_LightIndexList = nullptr;
			Buffer* m_LightGrid = nullptr;

			struct
			{
				Texture* m_Albedo = nullptr;
				Texture* m_Normals = nullptr;
				Texture* m_Emissive = nullptr;
				Texture* m_PBRConsts = nullptr;
				Texture* m_Depth = nullptr;
			} m_GBuffer;

			Texture* m_AmbientOcclusionFactors = nullptr;
			Texture* m_LightAccumulationBuffer = nullptr;
			Texture* m_TempPostFxSurface = nullptr;
		} m_Transient;

		inline Texture* GetPostFXSrc() const { return m_Persistent.m_FinalOutput; }
		inline Texture* GetPostFXDst() const { return m_Transient.m_TempPostFxSurface; }
		inline void SwapPostFXSurfaces() { std::swap(m_Persistent.m_FinalOutput, m_Transient.m_TempPostFxSurface); }
	};
}