#include "core/precomp.h"
#include "core/camera/camerafrustum.h"
#include "core/boundingvolume.h"

namespace Khan
{
	void CameraFrustum::Update(const glm::mat4& viewProjMatrix)
	{
		glm::vec4 plane;

		plane[0] = viewProjMatrix[3][0] + viewProjMatrix[0][0];
		plane[1] = viewProjMatrix[3][1] + viewProjMatrix[0][1];
		plane[2] = viewProjMatrix[3][2] + viewProjMatrix[0][2];
		plane[3] = viewProjMatrix[3][3] + viewProjMatrix[0][3];
		m_Planes[FrustumPlane::Left] = -glm::normalize(plane);

		plane[0] = viewProjMatrix[3][0] - viewProjMatrix[0][0];
		plane[1] = viewProjMatrix[3][1] - viewProjMatrix[0][1];
		plane[2] = viewProjMatrix[3][2] - viewProjMatrix[0][2];
		plane[3] = viewProjMatrix[3][3] - viewProjMatrix[0][3];
		m_Planes[FrustumPlane::Right] = -glm::normalize(plane);

		plane[0] = viewProjMatrix[3][0] - viewProjMatrix[1][0];
		plane[1] = viewProjMatrix[3][1] - viewProjMatrix[1][1];
		plane[2] = viewProjMatrix[3][2] - viewProjMatrix[1][2];
		plane[3] = viewProjMatrix[3][3] - viewProjMatrix[1][3];
		m_Planes[FrustumPlane::Top] = -glm::normalize(plane);

		plane[0] = viewProjMatrix[3][0] + viewProjMatrix[1][0];
		plane[1] = viewProjMatrix[3][1] + viewProjMatrix[1][1];
		plane[2] = viewProjMatrix[3][2] + viewProjMatrix[1][2];
		plane[3] = viewProjMatrix[3][3] + viewProjMatrix[1][3];
		m_Planes[FrustumPlane::Bottom] = -glm::normalize(plane);

		plane[0] = viewProjMatrix[2][0];
		plane[1] = viewProjMatrix[2][1];
		plane[2] = viewProjMatrix[2][2];
		plane[3] = viewProjMatrix[2][3];
		m_Planes[FrustumPlane::Near] = -glm::normalize(plane);

		plane[0] = viewProjMatrix[3][0] + viewProjMatrix[2][0];
		plane[1] = viewProjMatrix[3][1] + viewProjMatrix[2][1];
		plane[2] = viewProjMatrix[3][2] + viewProjMatrix[2][2];
		plane[3] = viewProjMatrix[3][3] + viewProjMatrix[2][3];
		m_Planes[FrustumPlane::Far] = -glm::normalize(plane);
	}

	bool CameraFrustum::Cull(const BoundingVolume& bv) const
	{
		switch (bv.GetType())
		{
			case BoundingVolume::Type::Sphere:
				{
					const glm::vec4 center(bv.GetSphereCenter(), 1.0f);
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
					glm::vec4 min(bv.GetAABBoxGlobalMin());
					glm::vec4 max(bv.GetAABBoxGlobalMax());

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
					glm::vec4 min(bv.GetOOBBoxMin(), 1.0f);
					glm::vec4 max(bv.GetAABBoxMax(), 1.0f);
					glm::mat4 oobbMatrix = bv.GetParentMatrix();

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

	KH_FORCE_INLINE bool CameraFrustum::CullPlaneSphere(const glm::vec4& plane, const glm::vec4& center, float radius) const
	{
		float dist = glm::dot(plane, center);
		return dist > radius;
	}

	KH_FORCE_INLINE bool CameraFrustum::CullPlaneAABBox(const glm::vec4& plane, const glm::vec4& min, const glm::vec4& max) const
	{
		glm::vec4 boxPoint(plane[0] < 0.0f ? max[0] : min[0],
			plane[1] < 0.0f ? max[1] : min[1],
			plane[2] < 0.0f ? max[2] : min[2],
			1.0f);

		float dist = glm::dot(plane, boxPoint);
		return dist > 0.0f;
	}

	KH_FORCE_INLINE bool CameraFrustum::CullPlaneOOBBox(const glm::vec4& plane, const glm::mat4& matrix, const glm::vec4& min, const glm::vec4& max) const
	{
		glm::mat4 invMatrix = glm::transpose(matrix);
		glm::vec4 localPlane(invMatrix * plane);

		return CullPlaneAABBox(localPlane, min, max);
	}
}