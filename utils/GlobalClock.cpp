#include "GlobalClock.h"

sf::Clock & GlobalClock::getClock()
{
	static sf::Clock clock;
	return clock;
}
