#pragma once
#include "core/boundingvolume.h"
#include "core/primitivetypes.h"
#include "graphicshal/constantbuffer.hpp"

namespace Khan
{
	class Buffer;
	class Material;

	struct Mesh
	{
		Mesh();

		Buffer* m_VertexBuffer;
		Buffer* m_IndexBuffer;
		uint32 m_VertexCount;
		uint32 m_IndexCount;
		Material* m_Material;
		BoundingVolume m_AABB;
		ConstantBuffer m_ParentTransform;
		float m_DistanceFromCamera;
	};
}