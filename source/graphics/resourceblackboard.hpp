#pragma once

namespace Khan
{
	class Buffer;
	class Texture;

	struct ResourceBlackboard
	{
		struct
		{
			Buffer* m_ScreenFrustums = nullptr;
			Texture* m_FinalOutput = nullptr;
		} m_Persistent;

		struct
		{
			Buffer* m_ActiveSceneLights = nullptr;
			Buffer* m_OpaqueLightIndexList = nullptr;
			Buffer* m_TransparentLightIndexList = nullptr;
			Texture* m_OpaqueLightGrid = nullptr;
			Texture* m_TransparentLightGrid = nullptr;

			struct
			{
				Texture* m_Albedo = nullptr;
				Texture* m_Normals = nullptr;
				Texture* m_Emissive = nullptr;
				Texture* m_SpecularReflectance = nullptr;
				Texture* m_MetallicAndRoughness = nullptr;
				Texture* m_MotionVectors = nullptr;
				Texture* m_Depth = nullptr;
			} m_GBuffer;

			Texture* m_LightAccumulationBuffer = nullptr;
		} m_Transient;
	};
}