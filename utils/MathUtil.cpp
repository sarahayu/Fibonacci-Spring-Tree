#include "MathUtil.h"

const sf::Vector3f MathUtil::rotate(const sf::Vector3f & point, const sf::Vector2f & angle)
{
	const sf::Vector3f rotatedZ(
		point.x * std::cos(angle.y) - point.y * std::sin(angle.y),
		point.y * std::cos(angle.y) + point.x * std::sin(angle.y),
		point.z);
	return sf::Vector3f(
		rotatedZ.x * std::cos(angle.x) - rotatedZ.z * std::sin(angle.x),
		rotatedZ.y,
		rotatedZ.z * std::cos(angle.x) + rotatedZ.x * std::sin(angle.x));
}

const glm::vec3 MathUtil::getSunPos(const float & azimuth)
{
	return{ std::cos(azimuth),0.7f,std::sin(azimuth) };
}

const float MathUtil::lerp(const float & a, const float & b, const float & f)
{
	return a + f * (b - a);
}

const glm::vec3 MathUtil::toGLM3(const sf::Vector3f & vec)
{
	return{ vec.x, vec.y, vec.z };
}

const sf::Vector3f MathUtil::toSF3(const glm::vec3 & vec)
{
	return{ vec.x, vec.y, vec.z };
}

const glm::vec2 MathUtil::toGLM2(const sf::Vector2i & vec)
{
	return{ vec.x, vec.y };
}

const glm::vec2 MathUtil::toGLM2(const sf::Vector2f & vec)
{
	return{ vec.x, vec.y };
}

const sf::Vector2f MathUtil::toSF2(const glm::vec2 & vec)
{
	return{ vec.x, vec.y };
}

const sf::Vector2f MathUtil::toSF2(const ImVec2 & vec)
{
	return{ vec.x, vec.y };
}
