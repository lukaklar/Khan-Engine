#include "core/precomp.h"
#include "core/camera/camera.h"
#include <thirdparty/glm/glm.hpp>
#include <thirdparty/glm/gtx/transform.hpp>

namespace Khan
{
	Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_Projection(glm::perspective(fov, aspectRatio, nearClip, farClip))
		, m_View(1.0f)
		, m_ViewProjection(m_Projection)
		, m_Position(0.0f)
	{
	}
}