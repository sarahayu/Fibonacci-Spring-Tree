#pragma once
#include <glm\glm.hpp>
#include <SFML\Graphics.hpp>
#include "..\imgui\imgui.h"

const float PI = 3.14159265f;
const float TWO_PI = PI * 2;

namespace MathUtil
{
	const sf::Vector3f rotate(const sf::Vector3f &point, const sf::Vector2f &angle);
	const glm::vec3 getSunPos(const float &azimuth);
	const float lerp(const float &a, const float &b, const float &f);

	const glm::vec3 toGLM3(const sf::Vector3f &vec);
	const sf::Vector3f toSF3(const glm::vec3 &vec);
	const glm::vec2 toGLM2(const sf::Vector2i &vec);
	const glm::vec2 toGLM2(const sf::Vector2f &vec);
	const sf::Vector2f toSF2(const glm::vec2 &vec);
	const sf::Vector2f toSF2(const ImVec2 &vec);
}