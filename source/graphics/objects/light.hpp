#pragma once

namespace Khan
{
	class Light
	{
	public:
		enum Type
		{
			Directional,
			Point,
			Spot,
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

	class PointLight : public Light
	{
	public:
		PointLight() : Light(Point) {}

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline void SetPosition(const glm::vec3& value) { m_Position = value; }
		inline float GetRange() const { return m_Range; }
		inline void SetRange(float value) { m_Range = value; }

	private:
		glm::vec3 m_Position;
		float m_Range;
	};

	class SpotLight : public Light
	{
	public:
		SpotLight() : Light(Spot) {}

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline void SetPosition(const glm::vec3& value) { m_Position = value; }
		inline const glm::vec3& GetDirection() const { return m_Direction; }
		inline void SetDirection(const glm::vec3& value) { m_Direction = value; }
		inline float GetCutOff() const { return m_CutOff; }
		inline void SetCutOff(float value) { m_CutOff = value; }
		inline float GetOuterCutOff() const { return m_OuterCutOff; }
		inline void SetOuterCutOff(float value) { m_OuterCutOff = value; }

	private:
		glm::vec3 m_Position;
		glm::vec3 m_Direction;
		float m_CutOff, m_OuterCutOff;
	};
}