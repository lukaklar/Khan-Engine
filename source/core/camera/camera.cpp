#include "core/precomp.h"
#include "core/camera/camera.hpp"
#include "core/ecs/entity.hpp"
#include "core/math.h"

namespace Khan
{
	Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov)
		, m_AspectRatio(aspectRatio)
		, m_NearClip(nearClip)
		, m_FarClip(farClip)
		, m_Projection(Perspective(Radians(fov), aspectRatio, nearClip, farClip))
		, m_ViewMatrix(1.0f)
		, m_ViewProjection(m_Projection)
		, m_InverseProjection(Inverse(m_Projection))
	{
	}

	void Camera::Update(float dt)
	{
		const Quaternion& orientation = m_Target->GetGlobalOrientation();
		Vector3 direction(orientation.x, orientation.y, orientation.z);
		Vector3 position = m_Target->GetGlobalPosition().xyz();
		m_ViewMatrix = LookAt(position, position + direction, Vector3(0.0f, 1.0f, 0.0f));

		m_ViewProjection = m_Projection * m_ViewMatrix;

		m_Frustum.Update(m_ViewProjection);
	}

	void Camera::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportHeight = width;
		m_ViewportHeight = height;

		m_AspectRatio = static_cast<float>(m_ViewportWidth) / static_cast<float>(m_ViewportHeight);

		m_Projection = Perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);
		m_InverseProjection = Inverse(m_Projection);
	}
}