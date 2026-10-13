#include "core/precomp.h"
#include "core/camera/camerafrustum.hpp"
#include "core/boundingvolume.h"

namespace Khan
{
	inline static Vector4 GetNormalizedPlane(const Vector4& plane)
	{
		float length3 = Sqrt(plane.x * plane.x + plane.y * plane.y + plane.z * plane.z);
		return plane / length3;
	}

	void CameraFrustum::Update(const Matrix4& viewProjMatrix)
	{
		Vector4 plane;

		plane[0] = viewProjMatrix[0][3] + viewProjMatrix[0][0];
		plane[1] = viewProjMatrix[1][3] + viewProjMatrix[1][0];
		plane[2] = viewProjMatrix[2][3] + viewProjMatrix[2][0];
		plane[3] = viewProjMatrix[3][3] + viewProjMatrix[3][0];
		m_Planes[FrustumPlane::Left] = -GetNormalizedPlane(plane);

		plane[0] = viewProjMatrix[0][3] - viewProjMatrix[0][0];
		plane[1] = viewProjMatrix[1][3] - viewProjMatrix[1][0];
		plane[2] = viewProjMatrix[2][3] - viewProjMatrix[2][0];
		plane[3] = viewProjMatrix[3][3] - viewProjMatrix[3][0];
		m_Planes[FrustumPlane::Right] = -GetNormalizedPlane(plane);

		plane[0] = viewProjMatrix[0][3] - viewProjMatrix[0][1];
		plane[1] = viewProjMatrix[1][3] - viewProjMatrix[1][1];
		plane[2] = viewProjMatrix[2][3] - viewProjMatrix[2][1];
		plane[3] = viewProjMatrix[3][3] - viewProjMatrix[3][1];
		m_Planes[FrustumPlane::Top] = -GetNormalizedPlane(plane);

		plane[0] = viewProjMatrix[0][3] + viewProjMatrix[0][1];
		plane[1] = viewProjMatrix[1][3] + viewProjMatrix[1][1];
		plane[2] = viewProjMatrix[2][3] + viewProjMatrix[2][1];
		plane[3] = viewProjMatrix[3][3] + viewProjMatrix[3][1];
		m_Planes[FrustumPlane::Bottom] = -GetNormalizedPlane(plane);

		plane[0] = viewProjMatrix[0][2];
		plane[1] = viewProjMatrix[1][2];
		plane[2] = viewProjMatrix[2][2];
		plane[3] = viewProjMatrix[3][2];
		m_Planes[FrustumPlane::Near] = -GetNormalizedPlane(plane);

		plane[0] = viewProjMatrix[0][3] + viewProjMatrix[0][2];
		plane[1] = viewProjMatrix[1][3] + viewProjMatrix[1][2];
		plane[2] = viewProjMatrix[2][3] + viewProjMatrix[2][2];
		plane[3] = viewProjMatrix[3][3] + viewProjMatrix[3][2];
		m_Planes[FrustumPlane::Far] = -GetNormalizedPlane(plane);
	}

	bool CameraFrustum::Cull(const BoundingVolume& bv) const
	{
		switch (bv.GetType())
		{
			case BoundingVolume::Type::Sphere:
				{
					const Vector4 center(bv.GetSphereCenter(), 1.0f);
					float radius = bv.GetSphereRadius();

					for (uint32_t i = 0; i < FrustumPlane::Count; ++i)
					{
						if (CullPlaneSphere(m_Planes[i], center, radius))
						{
							return true;
						}
					}
				}
				break;
			case BoundingVolume::Type::AABBox:
				{
					Vector4 min(bv.GetAABBoxGlobalMin());
					Vector4 max(bv.GetAABBoxGlobalMax());

					for (uint32_t i = 0; i < FrustumPlane::Count; ++i)
					{
						if (CullPlaneAABBox(m_Planes[i], min, max))
						{
							return true;
						}
					}
				}
				break;
			case BoundingVolume::Type::OOBBox:
				{
					Vector4 min(bv.GetOOBBoxMin(), 1.0f);
					Vector4 max(bv.GetAABBoxMax(), 1.0f);
					Matrix4 oobbMatrix = bv.GetParentMatrix();

					for (uint32_t i = 0; i < FrustumPlane::Count; ++i)
					{
						if (CullPlaneOOBBox(m_Planes[i], oobbMatrix, min, max))
						{
							return true;
						}
					}
				}
				break;
		}

		return false;
	}

	KH_FORCE_INLINE bool CameraFrustum::CullPlaneSphere(const Vector4& plane, const Vector4& center, float radius) const
	{
		float dist = Dot(plane, center);
		return dist > radius;
	}

	KH_FORCE_INLINE bool CameraFrustum::CullPlaneAABBox(const Vector4& plane, const Vector4& min, const Vector4& max) const
	{
		Vector4 boxPoint(plane[0] < 0.0f ? max[0] : min[0],
			plane[1] < 0.0f ? max[1] : min[1],
			plane[2] < 0.0f ? max[2] : min[2],
			1.0f);

		float dist = Dot(plane, boxPoint);
		return dist > 0.0f;
	}

	KH_FORCE_INLINE bool CameraFrustum::CullPlaneOOBBox(const Vector4& plane, const Matrix4& matrix, const Vector4& min, const Vector4& max) const
	{
		Matrix4 invMatrix = Transpose(matrix);
		Vector4 localPlane(invMatrix * plane);

		return CullPlaneAABBox(localPlane, min, max);
	}
}