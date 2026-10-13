#pragma once
#include "core/math.h"

namespace Khan
{
	class BoundingVolume
	{
	public:
		enum Type
		{
			Sphere,
			AABBox,
			OOBBox
		};

		inline Type GetType() const { return m_Type; }
		inline void SetType(Type value) { m_Type = value; }
		inline const Vector3& GetSphereCenter() const { return m_Min; }
		inline void SetSphereCenter(const Vector3& value) { m_Min = value; }
		inline float GetSphereRadius() const { return m_Max[0]; }
		inline void SetSphereRadius(float value) { m_Max[0] = value; }
		inline const Vector3& GetAABBoxMin() const { return m_Min; }
		inline void SetAABBoxMin(const Vector3& value) { m_Min = value; }
		inline const Vector3& GetAABBoxMax() const { return m_Max; }
		inline void SetAABBoxMax(const Vector3& value) { m_Max = value; }
		inline const Vector3& GetOOBBoxMin() const { return m_Min; }
		inline void SetOOBBoxMin(const Vector3& value) { m_Min = value; }
		inline const Vector3& GetOOBBoxMax() const { return m_Max; }
		inline void SetOOBBoxMax(const Vector3& value) { m_Max = value; }

		Matrix4 GetOOBBoxMatrix() const;
		inline void SetOOBBoxMatrix(Matrix4& value) { m_ParentMatrix = &value; }
		
		Matrix4 GetParentMatrix() const;
		void SetParentMatrixPtr(const Matrix4* matrix) { m_ParentMatrix = matrix; }

		Vector4 GetAABBoxGlobalMin() const;
		void SetAABBoxGlobalMin(const Vector3& value);
		Vector4 GetAABBoxGlobalMax() const;
		void SetAABBoxGlobalMax(const Vector3& value);

	private:
		Type m_Type;
		Vector3 m_Min;
		Vector3 m_Max;
		const Matrix4* m_ParentMatrix;
	};
}