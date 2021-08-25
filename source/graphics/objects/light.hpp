#pragma once
#include <thirdparty/glm/glm.hpp>

namespace Khan
{
	class Light
	{
	public:
		enum Type
		{
			Directional,
			Omni,
			Spot,
			Area
		};

		virtual ~Light() = 0;

		inline Type GetType() const { return m_Type; }
		inline const glm::vec3& GetColor() const { return m_Color; }
		inline void SetColor(const glm::vec3& value) { m_Color = value; }
		inline float GetLuminance() const { return m_Luminance; }
		inline void SetLuminance(float value) { m_Luminance = value; }
		inline bool IsActive() const { return m_Active; }
		inline void SetActive(bool value) { m_Active = value; }

	protected:
		Light(Type type) : m_Type(type) {}

		const Type m_Type;
		glm::vec3 m_Color;
		float m_Luminance;
		bool m_Active;
	};

	class DirectionalLight : public Light
	{
	public:
		DirectionalLight() : Light(Directional) {}

		inline const glm::vec3& GetDirection() const { return m_Direction; }
		inline void SetDirection(const glm::vec3& value) { m_Direction = value; }

	private:
		glm::vec3 m_Direction;
	};

	class OmniLight : public Light
	{
	public:
		OmniLight() : Light(Omni) {}

		inline float GetRadius() const { return m_Radius; }
		inline void SetRadius(float value) { m_Radius = value; }

	private:
		float m_Radius;
	};

	class SpotLight : public Light
	{
	public:
		SpotLight() : Light(Spot) {}

		inline float GetRange() const { return m_Range; }
		inline void SetRange(float value) { m_Range = value; }
		inline const glm::vec3& GetDirection() const { return m_Direction; }
		inline void SetDirection(const glm::vec3& value) { m_Direction = value; }
		inline float GetAngle() const { return m_Angle; }
		inline void SetAngle(float value) { m_Angle = value; }

	private:
		glm::vec3 m_Direction;
		float m_Angle;
		float m_Range;
	};

	class AreaLight : public Light
	{
	public:
		AreaLight() : Light(Area) {}

		inline float GetAreaWidth() const { return m_AreaWidth; }
		inline void SetAreaWidth(float value) { m_AreaWidth = value; }
		inline float GetAreaHeight() const { return m_AreaHeight; }
		inline void SetAreaHeight(float value) { m_AreaHeight = value; }

	private:
		float m_AreaWidth;
		float m_AreaHeight;
	};
}