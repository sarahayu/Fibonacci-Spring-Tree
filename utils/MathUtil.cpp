#include "MathUtil.h"

const sf::Vector3f rotate(const sf::Vector3f & point, const sf::Vector2f & angle)
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

const glm::vec3 getSunPos(const float & azimuth)
{
	return{ std::cos(azimuth),0.7f,std::sin(azimuth) };
}
