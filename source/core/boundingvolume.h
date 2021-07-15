#pragma once
#include <thirdparty/glm/glm.hpp>

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
		inline const glm::vec3& GetSphereCenter() const { return m_Min; }
		inline void SetSphereCenter(const glm::vec3& value) { m_Min = value; }
		inline float GetSphereRadius() const { return m_Max[0]; }
		inline void SetSphereRadius(float value) { m_Max[0] = value; }
		inline const glm::vec3& GetAABBoxMin() const { return m_Min; }
		inline void SetAABBoxMin(const glm::vec3& value) { m_Min = value; }
		inline const glm::vec3& GetAABBoxMax() const { return m_Max; }
		inline void SetAABBoxMax(const glm::vec3& value) { m_Max = value; }
		inline const glm::vec3& GetOOBBoxMin() const { return m_Min; }
		inline void SetOOBBoxMin(const glm::vec3& value) { m_Min = value; }
		inline const glm::vec3& GetOOBBoxMax() const { return m_Max; }
		inline void SetOOBBoxMax(const glm::vec3& value) { m_Max = value; }

		glm::mat4 GetOOBBoxMatrix() const;
		inline void SetOOBBoxMatrix(glm::mat4& value) { m_ParentMatrix = &value; }
		
		glm::mat4 GetParentMatrix() const;
		void SetParentMatrixPtr(const glm::mat4* matrix) { m_ParentMatrix = matrix; }

		glm::vec4 GetAABBoxGlobalMin() const;
		void SetAABBoxGlobalMin(const glm::vec3& value);
		glm::vec4 GetAABBoxGlobalMax() const;
		void SetAABBoxGlobalMax(const glm::vec3& value);

	private:
		Type m_Type;
		glm::vec3 m_Min;
		glm::vec3 m_Max;
		const glm::mat4* m_ParentMatrix;
	};
}