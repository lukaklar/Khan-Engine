#pragma once
#include "core/math.h"

namespace Khan
{
	class BoundingVolume;

	class CameraFrustum
	{
	public:
		enum FrustumPlane
		{
			Left,
			Right,
			Top,
			Bottom,
			Near,
			Far,
			Count
		};

		void Update(const Matrix4& viewProjMatrix);

		bool Cull(const BoundingVolume& bv) const;

	private:
		bool CullPlaneSphere(const Vector4& plane, const Vector4& center, float radius) const;
		bool CullPlaneAABBox(const Vector4& plane, const Vector4& min, const Vector4& max) const;
		bool CullPlaneOOBBox(const Vector4& plane, const Matrix4& matrix, const Vector4& min, const Vector4& max) const;

		Vector4 m_Planes[FrustumPlane::Count];
	};
}