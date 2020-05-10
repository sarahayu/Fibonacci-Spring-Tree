#pragma once
#include <SFML\Graphics.hpp>

class GlobalClock
{
public:
	static sf::Clock& getClock();
};