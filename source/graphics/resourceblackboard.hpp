#pragma once

namespace Khan
{
	class Buffer;
	class Texture;

	struct ResourceBlackboard
	{
		// TODO: Create these in the renderer
		Buffer* m_ScreenFrustums;
		Buffer* m_SceneLights;

		Buffer* m_OpaqueLightIndexList;
		Buffer* m_TransparentLightIndexList;
		Texture* m_OpaqueLightGrid;
		Texture* m_TransparentLightGrid;

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