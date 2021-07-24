#pragma once
#include "core/camera/camerafrustum.hpp"

namespace Khan
{
	class Camera
	{
	public:
		Camera(float fov, float aspectRatio, float nearClip, float farClip);

		inline const CameraFrustum& GetFrustum() const { return m_Frustum; }

		inline const glm::mat4& GetProjection() const { return m_Projection; }
		inline const glm::mat4& GetView() const { return m_View; }
		inline const glm::mat4& GetViewProjection() const { return m_ViewProjection; }

		inline const glm::vec3& GetPosition() const { return m_Position; }

		void Update(float dt);

	protected:
		CameraFrustum m_Frustum;

		glm::mat4 m_Projection;
		glm::mat4 m_View;
		glm::mat4 m_ViewProjection;

		glm::vec3 m_Position;
		glm::vec3 m_FocalPoint;
		float m_Pitch, m_Yaw, m_Roll;

		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;
		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
	};
}