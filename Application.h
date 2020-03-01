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
	std::vector<sf::Vector3f> m_treeBranchPos;
	std::vector<int> m_treeGenerations;
	sf::VertexArray m_treeVertices;

	struct {
		int fibStart = 1;
		int iterations = 12;
		float angle = TWO_PI * 1 / 8;
		float angleDecreaseFactor = 0.97f;
		float displacementAngle = TWO_PI * 1 / 48;
		float length = 50.f;
		float lengthDecreaseFactor = 0.9f;
	} m_input;
};