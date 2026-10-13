#include "core/precomp.h"
#include "core/boundingvolume.h"

namespace Khan
{
	Matrix4 BoundingVolume::GetOOBBoxMatrix() const
	{
		if (m_Type == Type::OOBBox)
		{
			return GetParentMatrix();
		}

		return Identity<Matrix4>();
	}

	Matrix4 BoundingVolume::GetParentMatrix() const
	{
		if (m_ParentMatrix)
			return *m_ParentMatrix;
		else
			return Identity<Matrix4>();
	}

	Vector4 BoundingVolume::GetAABBoxGlobalMin() const
	{
		Vector4 globalMin(m_Min, 1.0f);

		if (m_ParentMatrix)
		{
			globalMin += (*m_ParentMatrix)[3];
		}

		return globalMin;
	}

	void BoundingVolume::SetAABBoxGlobalMin(const Vector3& value)
	{
		m_Min = m_ParentMatrix ? value - Vector3((*m_ParentMatrix)[3]) : value;
	}

	Vector4 BoundingVolume::GetAABBoxGlobalMax() const
	{
		Vector4 globalMax(m_Max, 1.0f);

		if (m_ParentMatrix)
		{
			globalMax += (*m_ParentMatrix)[3];
		}

		return globalMax;
	}

	void BoundingVolume::SetAABBoxGlobalMax(const Vector3& value)
	{
		m_Max = m_ParentMatrix ? value - Vector3((*m_ParentMatrix)[3]) : value;
	}
}