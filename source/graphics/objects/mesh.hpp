#pragma once
#include "core/boundingvolume.h"
#include "graphics/hal/constantbuffer.hpp"

namespace Khan
{
	class Buffer;
	class Material;

	struct Mesh
	{
		Mesh();

		Buffer* m_VertexBuffer;
		Buffer* m_IndexBuffer;
		uint32_t m_VertexCount;
		uint32_t m_IndexCount;
		Material* m_Material;
		BoundingVolume m_AABB;
		ConstantBuffer m_ParentTransform;
		float m_DistanceFromCamera;
	};
}