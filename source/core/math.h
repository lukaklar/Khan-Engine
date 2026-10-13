#pragma once

#include <thirdparty/glm/glm.hpp>
#include <thirdparty/glm/gtc/constants.hpp>
#include <thirdparty/glm/gtc/epsilon.hpp>
#include <thirdparty/glm/gtc/matrix_transform.hpp>
#include <thirdparty/glm/gtc/quaternion.hpp>
#include <thirdparty/glm/gtx/norm.hpp>
#include <thirdparty/glm/gtx/quaternion.hpp>
#include <thirdparty/glm/gtx/transform.hpp>

namespace Khan
{
	// Types
	using Vector2I = glm::ivec2;
	using Vector3I = glm::ivec3;
	using Vector4I = glm::ivec4;

	using Vector2U = glm::uvec2;
	using Vector3U = glm::uvec3;
	using Vector4U = glm::uvec4;

	using Vector2D = glm::dvec2;
	using Vector3D = glm::dvec3;
	using Vector4D = glm::dvec4;

	using Vector2F = glm::vec2;
	using Vector3F = glm::vec3;
	using Vector4F = glm::vec4;

	using Vector2 = Vector2F;
	using Vector3 = Vector3F;
	using Vector4 = Vector4F;

	using Matrix2F = glm::mat2;
	using Matrix3F = glm::mat3;
	using Matrix4F = glm::mat4;

	using Matrix2 = Matrix2F;
	using Matrix3 = Matrix3F;
	using Matrix4 = Matrix4F;

	using Matrix2D = glm::dmat2;
	using Matrix3D = glm::dmat3;
	using Matrix4D = glm::dmat4;

	using QuaternionF = glm::quat;
	using QuaternionD = glm::dquat;
	using Quaternion = QuaternionF;

	// Constants
	template<typename T>
	inline T Pi()
	{
		return glm::pi<T>();
	}

	template<typename T>
	inline T TwoPi()
	{
		return glm::two_pi<T>();
	}

	template<typename T>
	inline T HalfPi()
	{
		return glm::half_pi<T>();
	}

	template<typename T>
	inline T QuarterPi()
	{
		return glm::quarter_pi<T>();
	}

	template<typename T>
	inline T Epsilon()
	{
		return glm::epsilon<T>();
	}

	// Common mathematical functions
	template<typename T>
	inline auto Sqrt(const T& value)
	{
		return glm::sqrt(value);
	}

	template<typename T>
	inline auto Inversesqrt(const T& value)
	{
		return glm::inversesqrt(value);
	}

	template<typename T>
	inline auto Abs(const T& value)
	{
		return glm::abs(value);
	}

	template<typename T>
	inline auto Sign(const T& value)
	{
		return glm::sign(value);
	}

	template<typename T>
	inline auto Floor(const T& value)
	{
		return glm::floor(value);
	}

	template<typename T>
	inline auto Ceil(const T& value)
	{
		return glm::ceil(value);
	}

	template<typename T>
	inline auto Round(const T& value)
	{
		return glm::round(value);
	}

	template<typename T>
	inline auto Trunc(const T& value)
	{
		return glm::trunc(value);
	}

	template<typename T>
	inline auto Fract(const T& value)
	{
		return glm::fract(value);
	}

	template<typename T>
	inline auto Mod(const T& value, const T& divisor)
	{
		return glm::mod(value, divisor);
	}

	template<typename T>
	inline auto Modf(const T& value, T& integerPart)
	{
		return glm::modf(value, integerPart);
	}

	template<typename T>
	inline auto Min(const T& a, const T& b)
	{
		return glm::min(a, b);
	}

	template<typename T>
	inline auto Max(const T& a, const T& b)
	{
		return glm::max(a, b);
	}

	template<typename T>
	inline auto Pow(const T& base, const T& exponent)
	{
		return glm::pow(base, exponent);
	}

	template<typename T>
	inline auto Exp(const T& value)
	{
		return glm::exp(value);
	}

	template<typename T>
	inline auto Exp2(const T& value)
	{
		return glm::exp2(value);
	}

	template<typename T>
	inline auto Log(const T& value)
	{
		return glm::log(value);
	}

	template<typename T>
	inline auto Log2(const T& value)
	{
		return glm::log2(value);
	}

	template<typename T>
	inline auto Fma(const T& a, const T& b, const T& c)
	{
		return glm::fma(a, b, c);
	}

	template<typename T>
	inline auto IsNan(const T& value)
	{
		return glm::isnan(value);
	}

	template<typename T>
	inline auto IsInf(const T& value)
	{
		return glm::isinf(value);
	}

	template<typename T>
	inline auto IsFinite(const T& value)
	{
		return glm::isfinite(value);
	}

	// Trigonometry
	template<typename T>
	inline auto Sin(const T& radians)
	{
		return glm::sin(radians);
	}

	template<typename T>
	inline auto Cos(const T& radians)
	{
		return glm::cos(radians);
	}

	template<typename T>
	inline auto Tan(const T& radians)
	{
		return glm::tan(radians);
	}

	template<typename T>
	inline auto Asin(const T& value)
	{
		return glm::asin(value);
	}

	template<typename T>
	inline auto Acos(const T& value)
	{
		return glm::acos(value);
	}

	template<typename T>
	inline auto Atan(const T& value)
	{
		return glm::atan(value);
	}

	template<typename T>
	inline auto Atan2(const T& y, const T& x)
	{
		return glm::atan(y, x);
	}

	// Vector operations
	template<typename T>
	inline auto Dot(const T& a, const T& b)
	{
		return glm::dot(a, b);
	}

	template<typename T>
	inline auto Cross(const T& a, const T& b)
	{
		return glm::cross(a, b);
	}

	template<typename T>
	inline auto Length(const T& value)
	{
		return glm::length(value);
	}

	template<typename T>
	inline auto LengthSquared(const T& value)
	{
		return glm::length2(value);
	}

	template<typename T>
	inline auto Normalize(const T& value)
	{
		return glm::normalize(value);
	}

	template<typename T>
	inline auto Distance(const T& a, const T& b)
	{
		return glm::distance(a, b);
	}

	template<typename T>
	inline auto DistanceSquared(const T& a, const T& b)
	{
		return glm::distance2(a, b);
	}

	template<typename T>
	inline auto Reflect(const T& incident, const T& normal)
	{
		return glm::reflect(incident, normal);
	}

	template<typename T>
	inline auto Refract(const T& incident, const T& normal, float eta)
	{
		return glm::refract(incident, normal, eta);
	}

	template<typename T>
	inline auto FaceForward(const T& normal, const T& incident, const T& reference)
	{
		return glm::faceforward(normal, incident, reference);
	}

	template<typename T>
	inline auto Any(const T& value)
	{
		return glm::any(value);
	}

	template<typename T>
	inline auto All(const T& value)
	{
		return glm::all(value);
	}

	template<typename T>
	inline auto Not(const T& value)
	{
		return glm::not_(value);
	}

	template<typename T>
	inline auto NormalizeSafe(const T& value, const T& fallback)
	{
		return glm::dot(value, value) > 0.0f ? glm::normalize(value) : fallback;
	}

	// Interpolation and comparisons
	template<typename T, typename U>
	inline auto Mix(const T& a, const T& b, const U& t)
	{
		return glm::mix(a, b, t);
	}

	template<typename T, typename U>
	inline auto Lerp(const T& a, const T& b, const U& t)
	{
		return glm::mix(a, b, t);
	}

	template<typename T>
	inline auto Clamp(const T& value, const T& min, const T& max)
	{
		return glm::clamp(value, min, max);
	}

	template<typename T>
	inline auto Saturate(const T& value)
	{
		return glm::clamp(value, T(0), T(1));
	}

	template<typename T>
	inline auto Smoothstep(const T& edge0, const T& edge1, const T& value)
	{
		return glm::smoothstep(edge0, edge1, value);
	}

	template<typename T>
	inline auto Smootherstep(const T& edge0, const T& edge1, const T& value)
	{
		const T t = glm::clamp(( value - edge0 ) / ( edge1 - edge0 ), T(0), T(1));
		return t * t * t * ( t * ( t * T(6) - T(15) ) + T(10) );
	}

	template<typename T>
	inline auto Step(const T& edge, const T& value)
	{
		return glm::step(edge, value);
	}

	template<typename T>
	inline auto Equal(const T& a, const T& b)
	{
		return glm::equal(a, b);
	}

	template<typename T>
	inline auto NotEqual(const T& a, const T& b)
	{
		return glm::notEqual(a, b);
	}

	template<typename T>
	inline auto EpsilonEqual(const T& a, const T& b, float epsilon)
	{
		return glm::epsilonEqual(a, b, epsilon);
	}

	template<typename T>
	inline auto EpsilonNotEqual(const T& a, const T& b, float epsilon)
	{
		return glm::epsilonNotEqual(a, b, epsilon);
	}

	// Angles
	template<typename T>
	inline auto Radians(T degrees)
	{
		return glm::radians(degrees);
	}

	template<typename T>
	inline auto Degrees(T radians)
	{
		return glm::degrees(radians);
	}

	// Matrices
	template<typename T>
	inline auto Inverse(const T& matrix)
	{
		return glm::inverse(matrix);
	}

	template<typename T>
	inline auto Transpose(const T& matrix)
	{
		return glm::transpose(matrix);
	}

	template<typename T>
	inline auto Determinant(const T& matrix)
	{
		return glm::determinant(matrix);
	}

	template<typename T>
	inline auto MatrixCompMult(const T& a, const T& b)
	{
		return glm::matrixCompMult(a, b);
	}

	template<typename T>
	inline auto OuterProduct(const T& a, const T& b)
	{
		return glm::outerProduct(a, b);
	}

	template<typename T>
	inline auto Identity()
	{
		return glm::identity<T>();
	}

	// Transformations
	template<typename T, typename U>
	inline auto Translate(const T& matrix, const U& translation)
	{
		return glm::translate(matrix, translation);
	}

	template<typename T, typename U>
	inline auto Scale(const T& matrix, const U& scale)
	{
		return glm::scale(matrix, scale);
	}

	template<typename T, typename U, typename V>
	inline auto Rotate(const T& matrix, U angle, const V& axis)
	{
		return glm::rotate(matrix, angle, axis);
	}

	template<typename T, typename U>
	inline auto Rotate(T angle, const U& axis)
	{
		return glm::rotate(angle, axis);
	}

	inline Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
	{
		return glm::lookAt(eye, center, up);
	}

	inline Matrix4 LookAtLH(const Vector3& eye, const Vector3& center, const Vector3& up)
	{
		return glm::lookAtLH(eye, center, up);
	}

	inline Matrix4 LookAtRH(const Vector3& eye, const Vector3& center, const Vector3& up)
	{
		return glm::lookAtRH(eye, center, up);
	}

	template<typename T>
	inline Matrix4 Perspective(T fovRadians, T aspectRatio, T nearPlane, T farPlane)
	{
		return glm::perspective(fovRadians, aspectRatio, nearPlane, farPlane);
	}

	template<typename T>
	inline Matrix4 PerspectiveLH(T fovRadians, T aspectRatio, T nearPlane, T farPlane)
	{
		return glm::perspectiveLH(fovRadians, aspectRatio, nearPlane, farPlane);
	}

	template<typename T>
	inline Matrix4 PerspectiveRH(T fovRadians, T aspectRatio, T nearPlane, T farPlane)
	{
		return glm::perspectiveRH(fovRadians, aspectRatio, nearPlane, farPlane);
	}

	template<typename T>
	inline Matrix4 Orthographic(T left, T right, T bottom, T top, T nearPlane, T farPlane)
	{
		return glm::ortho(left, right, bottom, top, nearPlane, farPlane);
	}

	template<typename T>
	inline Matrix4 Frustum(T left, T right, T bottom, T top, T nearPlane, T farPlane)
	{
		return glm::frustum(left, right, bottom, top, nearPlane, farPlane);
	}

	// Quaternions
	template<typename T, typename U>
	inline auto Slerp(const T& a, const T& b, U t)
	{
		return glm::slerp(a, b, t);
	}

	template<typename T>
	inline auto Conjugate(const T& quaternion)
	{
		return glm::conjugate(quaternion);
	}

	template<typename T, typename U>
	inline auto AngleAxis(T angleRadians, const U& axis)
	{
		return glm::angleAxis(angleRadians, axis);
	}

	template<typename T>
	inline auto ToMatrix3(const T& quaternion)
	{
		return glm::mat3_cast(quaternion);
	}

	template<typename T>
	inline auto ToMatrix4(const T& quaternion)
	{
		return glm::mat4_cast(quaternion);
	}

	template<typename T>
	inline auto ToQuaternion(const T& matrix)
	{
		return glm::quat_cast(matrix);
	}

	template<typename T>
	inline auto QuaternionRotate(const T& quaternion, const Vector3& vector)
	{
		return quaternion * vector;
	}
}