#pragma once
#include <SFML\Graphics.hpp>
#include "TreeSkeleton.h"
#include "MathUtil.h"

class Application
{
public:
	Application();

	void run();

private:

	void input();
	void update(float deltatime);
	void draw();

	sf::RenderWindow m_window;
	TreeSkeleton m_treeSkeleton;

	struct {
		int fibStart = 1;
		int iterations = 3;
		float angle = TWO_PI * 1 / 8;
		float angleDecreaseFactor = 0.97f;
		float displacementAngle = TWO_PI * 1 / 48;
		float length = 30.f;
		float lengthDecreaseFactor = 0.9f;
	} m_input;
};