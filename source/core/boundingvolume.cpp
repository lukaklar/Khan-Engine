#include "core/precomp.h"
#include "core/boundingvolume.h"
#include <thirdparty/glm/gtx/transform.hpp>

namespace Khan
{
	glm::mat4 BoundingVolume::GetOOBBoxMatrix() const
	{
		if (m_Type == Type::OOBBox)
		{
			return GetParentMatrix();
		}

		return glm::identity<glm::mat4>();
	}

	glm::mat4 BoundingVolume::GetParentMatrix() const
	{
		if (m_ParentMatrix)
			return *m_ParentMatrix;
		else
			return glm::identity<glm::mat4>();
	}

	glm::vec4 BoundingVolume::GetAABBoxGlobalMin() const
	{
		glm::vec4 globalMin(m_Min, 1.0f);

		if (m_ParentMatrix)
		{
			globalMin += (*m_ParentMatrix)[3];
		}

		return globalMin;
	}

	void BoundingVolume::SetAABBoxGlobalMin(const glm::vec3& value)
	{
		m_Min = m_ParentMatrix ? value - glm::vec3((*m_ParentMatrix)[3]) : value;
	}

	glm::vec4 BoundingVolume::GetAABBoxGlobalMax() const
	{
		glm::vec4 globalMax(m_Max, 1.0f);

		if (m_ParentMatrix)
		{
			globalMax += (*m_ParentMatrix)[3];
		}

		return globalMax;
	}

	void BoundingVolume::SetAABBoxGlobalMax(const glm::vec3& value)
	{
		m_Max = m_ParentMatrix ? value - glm::vec3((*m_ParentMatrix)[3]) : value;
	}
}