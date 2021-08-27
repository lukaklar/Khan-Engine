#include "core/precomp.h"
#include "core/camera/camera.hpp"
#include "core/ecs/entity.hpp"
#include <thirdparty/glm/glm.hpp>
#include <thirdparty/glm/gtx/transform.hpp>

namespace Khan
{
	Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov)
		, m_AspectRatio(aspectRatio)
		, m_NearClip(nearClip)
		, m_FarClip(farClip)
		, m_Projection(glm::perspective(fov, aspectRatio, nearClip, farClip))
		, m_ViewMatrix(1.0f)
		, m_ViewProjection(m_Projection)
	{
	}

	void Camera::Update(float dt)
	{
		m_ViewMatrix = glm::translate(glm::vec3(m_Target->GetGlobalPosition())) * glm::toMat4(m_Target->GetGlobalOrientation());
		m_ViewMatrix = glm::inverse(m_ViewMatrix);

		m_ViewProjection = m_Projection * m_ViewMatrix;

		m_Frustum.Update(m_ViewProjection);
	}

	void Camera::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportHeight = width;
		m_ViewportHeight = height;

		m_AspectRatio = static_cast<float>(m_ViewportWidth) / static_cast<float>(m_ViewportHeight);

		m_Projection = glm::perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);
		m_InverseProjection = glm::inverse(m_Projection);
	}
}