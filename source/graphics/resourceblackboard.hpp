#pragma once

namespace Khan
{
	class Buffer;
	class Texture;

	struct ResourceBlackboard
	{
		Buffer* m_ScreenFrustums = nullptr;
		// TODO: Create this in the renderer
		Buffer* m_SceneLights = nullptr;

		Buffer* m_OpaqueLightIndexList = nullptr;
		Buffer* m_TransparentLightIndexList = nullptr;
		Texture* m_OpaqueLightGrid = nullptr;
		Texture* m_TransparentLightGrid = nullptr;

		struct
		{
			Texture* Albedo = nullptr;
			Texture* Normals = nullptr;
			Texture* Emissive = nullptr;
			Texture* SpecularReflectance = nullptr;
			Texture* MetallicAndRoughness = nullptr;
			Texture* MotionVectors = nullptr;
			Texture* Depth = nullptr;
		} GBuffer;

		Texture* m_LightAccumulationBuffer = nullptr;
		Texture* m_FinalOutput = nullptr;
	};
}