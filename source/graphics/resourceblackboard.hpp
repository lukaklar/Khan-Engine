#pragma once

namespace Khan
{
	class Texture;

	struct ResourceBlackboard
	{
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