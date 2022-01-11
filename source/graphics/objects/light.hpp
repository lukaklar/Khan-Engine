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
	};

	class OmniLight : public Light
	{
	public:
		OmniLight() : Light(Omni) {}

		inline float GetRadius() const { return m_Radius; }
		inline void SetRadius(float value) { m_Radius = value; }
		inline const glm::vec3& GetAttenuation() const { return m_Attenuation; }
		inline void SetAttenuation(const glm::vec3& value) { m_Attenuation = value; }

	private:
		float m_Radius;
		glm::vec3 m_Attenuation;
	};

	class SpotLight : public Light
	{
	public:
		SpotLight() : Light(Spot) {}

		inline float GetRange() const { return m_Range; }
		inline void SetRange(float value) { m_Range = value; }
		inline float GetInnerConeAngle() const { return m_InnerConeAngle; }
		inline void SetInnerConeAngle(float value) { m_InnerConeAngle = value; }
		inline float GetOuterConeAngle() const { return m_OuterConeAngle; }
		inline void SetOuterConeAngle(float value) { m_OuterConeAngle = value; }
		inline const glm::vec3& GetAttenuation() const { return m_Attenuation; }
		inline void SetAttenuation(const glm::vec3& value) { m_Attenuation = value; }

	private:
		float m_Range;
		float m_InnerConeAngle;
		float m_OuterConeAngle;
		glm::vec3 m_Attenuation;
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