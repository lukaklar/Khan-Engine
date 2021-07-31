#pragma once
#include "core/camera/camerafrustum.hpp"

namespace Khan
{
	class Entity;

	class Camera
	{
	public:
		Camera(float fov = 45.0f, float aspectRatio = 1.778f, float nearClip = 0.1f, float farClip = 1000.0f);

		void Update(float dt);

		inline Entity* GetTarget() const { return m_Target; }
		inline void SetTarget(Entity* value) { m_Target = value; }

		inline const CameraFrustum& GetFrustum() const { return m_Frustum; }

		inline const glm::mat4& GetProjection() const { return m_Projection; }
		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const glm::mat4& GetViewProjection() const { return m_ViewProjection; }
		inline const glm::mat4& GetInverseProjection() const { return m_InverseProjection; }

		inline float GetViewportWidth() const { return m_ViewportWidth; }
		inline float GetViewportHeight() const { return m_ViewportHeight; }
		void OnViewportResize(uint32_t width, uint32_t height);

	protected:
		Entity* m_Target = nullptr;

		CameraFrustum m_Frustum;

		glm::mat4 m_Projection;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjection;
		glm::mat4 m_InverseProjection;

		float m_FOV, m_AspectRatio, m_NearClip, m_FarClip;
		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
	};
}