#pragma once
#include "core/boundingvolume.h"

namespace Khan
{
	class Buffer;
	class Material;

	struct Mesh
	{
		Buffer* m_VertexBuffer;
		Buffer* m_IndexBuffer;
		uint32_t m_VertexCount;
		uint32_t m_IndexCount;
		Material* m_Material;
		BoundingVolume m_AABB;
		float m_DistanceFromCamera;
	};
}