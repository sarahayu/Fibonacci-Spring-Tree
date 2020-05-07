#pragma once
#include <glm\glm.hpp>
#include <SFML\Graphics.hpp>
#include "..\imgui\imgui.h"

const float PI = 3.14159265f;
const float TWO_PI = PI * 2;

const sf::Vector3f rotate(const sf::Vector3f &point, const sf::Vector2f &angle);
const glm::vec3 getSunPos(const float &azimuth);

template <typename T>
const T castIM2(const ImVec2 &vec);
template <typename T>
const T castSF2(const sf::Vector2f &vec);
template <typename T>
const T castGL2(const glm::vec2 &vec);

template <typename T>
const T castSF3(const sf::Vector3f &vec);
template <typename T>
const T castGL3(const glm::vec3 &vec);

template<typename T>
inline const T castIM2(const ImVec2 & vec)
{
	return{ vec.x, vec.y };
}

template<typename T>
inline const T castSF2(const sf::Vector2f & vec)
{
	return{ vec.x, vec.y };
}

template<typename T>
inline const T castGL2(const glm::vec2 & vec)
{
	return{ vec.x, vec.y };
}

template<typename T>
inline const T castSF3(const sf::Vector3f & vec)
{
	return{ vec.x, vec.y, vec.z };
}

template<typename T>
inline const T castGL3(const glm::vec3 & vec)
{
	return{ vec.x, vec.y, vec.z };
}
