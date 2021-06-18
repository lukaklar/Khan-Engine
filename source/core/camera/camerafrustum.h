#pragma once

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

		void Update(const glm::mat4& viewProjMatrix);

		bool Cull(const BoundingVolume& bv) const;

	private:
		bool CullPlaneSphere(const glm::vec4& plane, const glm::vec4& center, float radius) const;
		bool CullPlaneAABBox(const glm::vec4& plane, const glm::vec4& min, const glm::vec4& max) const;
		bool CullPlaneOOBBox(const glm::vec4& plane, const glm::mat4& matrix, const glm::vec4& min, const glm::vec4& max) const;

		glm::vec4 m_Planes[FrustumPlane::Count];
	};
}